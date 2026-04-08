import base64
from datetime import datetime

import gdb

from lvglgdb.lvgl.data_utils import safe_collect
from lvglgdb.value import CorruptedError


# Registry of simple subsystems: (dict_key, lvgl_accessor_method, label)
# accessor=None means the subsystem uses lvgl.<dict_key>().snapshots() pattern
SIMPLE_REGISTRY: list[tuple[str, str | None, str]] = [
    ("animations",          "anims",            "animations"),
    ("timers",              "timers",            "timers"),
    ("indevs",              "indevs",            "indevs"),
    ("groups",              "groups",            "groups"),
    ("draw_units",          "draw_units",        "draw units"),
    ("image_decoders",      "image_decoders",    "image decoders"),
    ("fs_drivers",          "fs_drivers",        "fs drivers"),
    ("image_header_cache",  None,                "image header cache"),
]


def _collect_simple(lvgl, dict_key: str, accessor: str | None, label: str) -> list:
    """Collect a simple subsystem using the registry entry.

    For entries with accessor != None: [x.snapshot().as_dict() for x in lvgl.<accessor>()]
    For entries with accessor == None: [s.as_dict() for s in lvgl.<dict_key>().snapshots()]
    """

    @safe_collect(label)
    def _inner():
        if accessor is not None:
            return [x.snapshot().as_dict() for x in getattr(lvgl, accessor)()]
        return [s.as_dict() for s in getattr(lvgl, dict_key)().snapshots()]

    return _inner()


def collect_all() -> dict:
    """Collect all LVGL runtime data into a JSON-compatible dict."""
    from lvglgdb.lvgl import curr_inst

    lvgl = curr_inst()

    data = {
        "meta": {
            "timestamp": datetime.now().astimezone().isoformat(),
            "lvgl_version": _get_lvgl_version(),
        },
        # Specialized collectors (complex logic, not registry-driven)
        "displays": _collect_displays(lvgl),
        "object_trees": _collect_object_trees(lvgl),
        "image_cache": _collect_image_cache(lvgl),
        "draw_tasks": _collect_draw_tasks(lvgl),
        "subjects": _collect_subjects(lvgl),
    }
    # Registry-driven simple collectors
    for dict_key, accessor, label in SIMPLE_REGISTRY:
        data[dict_key] = _collect_simple(lvgl, dict_key, accessor, label)
    return data


def _get_lvgl_version() -> str | None:
    """Try to read LVGL version from macros."""
    try:
        major = int(gdb.parse_and_eval("LVGL_VERSION_MAJOR"))
        minor = int(gdb.parse_and_eval("LVGL_VERSION_MINOR"))
        patch = int(gdb.parse_and_eval("LVGL_VERSION_PATCH"))
        return f"{major}.{minor}.{patch}"
    except gdb.error:
        return None


def _buf_to_dict(draw_buf) -> dict | None:
    """Convert an LVDrawBuf to a dict with base64 PNG image."""
    if draw_buf is None:
        return None
    try:
        cf_info = draw_buf.color_format_info()
        header = draw_buf.super_value("header")
        stride = int(header["stride"])
        height = int(header["h"])
        bpp = cf_info["bpp"]
        width = (stride * 8) // bpp if bpp else 0

        png_bytes = draw_buf.to_png_bytes()
        image_b64 = base64.b64encode(png_bytes).decode("ascii") if png_bytes else None

        return {
            "addr": hex(int(draw_buf)),
            "width": width,
            "height": height,
            "color_format": cf_info["name"],
            "data_size": int(draw_buf.super_value("data_size")),
            "image_base64": image_b64,
        }
    except Exception:
        return None


@safe_collect("displays")
def _collect_displays(lvgl) -> list:
    """Collect display info with framebuffer data."""
    result = []
    for disp in lvgl.displays():
        d = disp.snapshot().as_dict()
        d["buf_1"] = _buf_to_dict(disp.buf_1)
        d["buf_2"] = _buf_to_dict(disp.buf_2)
        result.append(d)
    return result


@safe_collect("object trees")
def _collect_object_trees(lvgl) -> list:
    """Collect object trees for all displays with layer name annotations."""
    from lvglgdb.lvgl.snapshot import Snapshot

    result = []
    for disp in lvgl.displays():
        addrs = disp.layer_addrs

        @Snapshot.fallback(layer_name=lambda s: addrs.get(int(s)))
        def _screen_snapshot(screen):
            snap = screen.snapshot(
                include_children=True, include_styles=True,
            ).as_dict()
            snap["layer_name"] = addrs.get(int(screen))
            return snap

        result.append({
            "display_addr": hex(int(disp)),
            "screens": safe_collect(disp.screens, _screen_snapshot),
        })
    return result


@safe_collect("image cache")
def _collect_image_cache(lvgl) -> list:
    """Collect image cache entries with optional decoded buffer previews."""
    cache = lvgl.image_cache()
    entries = cache.snapshots()
    result = []
    for snap in entries:
        d = snap.as_dict()
        # Try to get preview of decoded buffer
        d["preview_base64"] = None
        decoded_addr = d.get("decoded_addr")
        if decoded_addr and decoded_addr != "0x0":
            try:
                from lvglgdb.lvgl.draw.lv_draw_buf import LVDrawBuf
                buf = LVDrawBuf(gdb.Value(int(decoded_addr, 16)))
                png_bytes = buf.to_png_bytes()
                if png_bytes:
                    d["preview_base64"] = base64.b64encode(
                        png_bytes
                    ).decode("ascii")
            except Exception:
                pass
        result.append(d)
    return result


@safe_collect("draw tasks")
def _collect_draw_tasks(lvgl) -> list:
    """Collect draw tasks from each display's layer chain."""
    from lvglgdb.lvgl.draw.lv_draw_task import LVDrawTask
    result = []
    for disp in lvgl.displays():
        layer = disp.super_value("layer_head")
        while layer and int(layer):
            task_head = layer["draw_task_head"]
            if int(task_head):
                for t in LVDrawTask(task_head):
                    result.append(t.snapshot().as_dict())
            layer = layer["next"]
    return result


@safe_collect("subjects")
def _collect_subjects(lvgl) -> list:
    """Collect subjects from object event lists across all displays."""
    seen = set()
    result = []
    for disp in lvgl.displays():
        for screen in disp.screens:
            _collect_subjects_from_obj(screen, seen, result)
    return result


def _collect_subjects_from_obj(obj, seen, result):
    """Recursively collect subjects from an object's event list."""
    from lvglgdb.lvgl.core.lv_observer import LVSubject

    event_list = obj.event_list
    if event_list:
        for dsc in event_list:
            try:
                user_data = dsc.user_data
                if not int(user_data):
                    continue
                subject = LVSubject(user_data)
                addr = int(subject)
                if addr not in seen:
                    seen.add(addr)
                    result.append(subject.snapshot().as_dict())
            except Exception:
                # Per-dsc failure: skip this descriptor, continue others
                continue

    try:
        for child in obj.children:
            try:
                _collect_subjects_from_obj(child, seen, result)
            except Exception:
                # Per-child failure: skip this child, continue siblings
                continue
    except CorruptedError:
        # Children pointer unreadable: stop traversal for this subtree
        pass
