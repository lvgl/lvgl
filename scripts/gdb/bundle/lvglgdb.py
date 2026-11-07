# LVGL GDB plugin, packed into a single file.
#
# Load it in GDB with:
#     source lvglgdb.py
#
# GENERATED FILE - do not edit. The source is lvgl/scripts/gdb/lvglgdb/;
# regenerate this file with: python3 scripts/gdb/build_bundle.py
# fmt: off

VERSION = "9.6.0"
DIGEST = "2850d2d47d256f35"

FILES = {}

FILES['lvglgdb/__init__.py'] = """from ._version import __version__

from .value import Value, ValueInput, CorruptedValue, CorruptedError
from .lvgl import (
    curr_inst,
    LVDisplay,
    LVDrawBuf,
    LVDrawTask,
    DRAW_TASK_TYPE_NAMES,
    DRAW_TASK_STATE_NAMES,
    DRAW_UNIT_TYPE_NAMES,
    LVDrawUnit,
    LVList,
    LVObject,
    ObjStyle,
    LVStyle,
    StyleEntry,
    dump_obj_styles,
    dump_obj_info,
    style_prop_name,
    decode_selector,
    format_style_value,
    LVCache,
    LVCacheEntry,
    LVCacheLRURB,
    LVCacheLRURBIterator,
    LVCacheIteratorBase,
    LVImageCache,
    LVImageHeaderCache,
    create_cache_iterator,
    LVRedBlackTree,
    LVArray,
    LVEvent,
    LVEventDsc,
    LVEventList,
    event_code_name,
    EVENT_CODE_NAMES,
    LVAnim,
    LVTimer,
    LVImageDecoder,
    LVFsDrv,
    format_coord,
    LVIndev,
    INDEV_TYPE_NAMES,
    LVGroup,
    LVObjClass,
    OBJ_FLAG_NAMES,
    decode_obj_flags,
    OBJ_STATE_NAMES,
    decode_obj_states,
    LVSubject,
    LVObserver,
    SUBJECT_TYPE_NAMES,
)
from . import cmds as cmds

__all__ = [
    "__version__",
    "curr_inst",
    "LVDisplay",
    "LVDrawBuf",
    "LVDrawTask",
    "LVDrawUnit",
    "LVList",
    "LVCache",
    "LVRedBlackTree",
    "LVObject",
    "ObjStyle",
    "LVStyle",
    "StyleEntry",
    "dump_obj_styles",
    "dump_obj_info",
    "style_prop_name",
    "decode_selector",
    "format_style_value",
    "Value",
    "ValueInput",
    "CorruptedValue",
    "CorruptedError",
    "LVCacheEntry",
    "LVCacheLRURB",
    "LVCacheLRURBIterator",
    "LVCacheIteratorBase",
    "LVImageCache",
    "LVImageHeaderCache",
    "create_cache_iterator",
    "LVArray",
    "LVEvent",
    "LVEventDsc",
    "LVEventList",
    "event_code_name",
    "EVENT_CODE_NAMES",
    "LVAnim",
    "LVTimer",
    "LVImageDecoder",
    "LVFsDrv",
    "format_coord",
    "LVIndev",
    "INDEV_TYPE_NAMES",
    "LVGroup",
    "LVObjClass",
    "OBJ_FLAG_NAMES",
    "decode_obj_flags",
    "OBJ_STATE_NAMES",
    "decode_obj_states",
    "LVSubject",
    "LVObserver",
    "SUBJECT_TYPE_NAMES",
]
"""

FILES['lvglgdb/_version.py'] = """__version__ = "9.6.0"
"""

FILES['lvglgdb/cmds/__init__.py'] = """import gdb

from .core import (
    DumpObj,
    DumpIndev,
    DumpGroup,
    InfoObjClass,
    InfoSubject,
    InfoWidget,
    DumpWidget,
    DumpWidgetProps,
)
from .display import DumpDisplayBuf
from .draw import InfoDrawUnit, DumpDrawTask
from .misc import (
    InfoStyle,
    DumpCache,
    CheckPrefix,
    CheckCache,
    DumpAnim,
    DumpTimer,
    DumpImageDecoder,
    DumpFsDrv,
    InfoVersion,
)
from .dashboard import DumpDashboard
from .debugger import Debugger
from .drivers import Lvglobal

__all__ = []

# Set pagination off and python print-stack full
gdb.execute("set pagination off")
gdb.write("set pagination off\\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\\n")


# Debugger
Debugger()

# Dumps
DumpObj()
DumpDisplayBuf()
DumpCache()
CheckPrefix()
CheckCache()
DumpAnim()
DumpTimer()
DumpImageDecoder()
DumpFsDrv()
DumpIndev()
DumpGroup()
DumpDrawTask()
DumpWidget()  # prefix for 'dump widget props'
DumpWidgetProps()

# Infos
InfoStyle()
InfoDrawUnit()
InfoObjClass()
InfoSubject()
InfoVersion()
InfoWidget()

# Drivers
Lvglobal()

# Dashboard
DumpDashboard()
"""

FILES['lvglgdb/cmds/core/__init__.py'] = """from .lv_obj import DumpObj
from .lv_indev import DumpIndev
from .lv_group import DumpGroup
from .lv_obj_class import InfoObjClass
from .lv_observer import InfoSubject
from .lv_widget import InfoWidget, DumpWidget, DumpWidgetProps

__all__ = [
    "DumpObj",
    "DumpIndev",
    "DumpGroup",
    "InfoObjClass",
    "InfoSubject",
    "InfoWidget",
    "DumpWidget",
    "DumpWidgetProps",
]
"""

FILES['lvglgdb/cmds/core/lv_group.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_group import LVGroup
from lvglgdb.lvgl.formatter import print_spec_table


class DumpGroup(gdb.Command):
    \"\"\"dump all focus groups\"\"\"

    def __init__(self):
        super(DumpGroup, self).__init__(
            "dump group", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        groups = curr_inst().groups()
        snaps = LVGroup.snapshots(groups)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVGroup._DISPLAY_SPEC["empty_msg"])
"""

FILES['lvglgdb/cmds/core/lv_indev.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_indev import LVIndev
from lvglgdb.lvgl.formatter import print_spec_table


class DumpIndev(gdb.Command):
    \"\"\"dump all input devices\"\"\"

    def __init__(self):
        super(DumpIndev, self).__init__(
            "dump indev", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        indevs = curr_inst().indevs()
        snaps = LVIndev.snapshots(indevs)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVIndev._DISPLAY_SPEC["empty_msg"])
"""

FILES['lvglgdb/cmds/core/lv_obj.py'] = """import argparse

import gdb

from .lv_widget import print_all_widgets, print_tree, resolve_widget


class DumpObj(gdb.Command):
    \"\"\"dump obj tree from specified obj\"\"\"

    def __init__(self):
        super(DumpObj, self).__init__(
            "dump obj", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl obj tree.")
        parser.add_argument(
            "-L",
            "--level",
            type=int,
            default=None,
            help="Limit the depth of the tree.",
        )
        parser.add_argument(
            "root",
            type=str,
            nargs="?",
            default=None,
            help="Optional root widget: a name, a name path, or an expression.",
        )
        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        # The listing is `info widget`'s, so both commands print the same tree
        # and there is only one renderer to keep current with the widget data.
        if args.root:
            root, error = resolve_widget(args.root)
            if root is None:
                print(f"Error: {error}")
                return
            print_tree(root, limit=args.level)
        else:
            print_all_widgets(limit=args.level)
"""

FILES['lvglgdb/cmds/core/lv_obj_class.py'] = """import argparse

import gdb

from lvglgdb.lvgl.core.lv_obj_class import LVObjClass
from lvglgdb.lvgl.formatter import print_info, print_spec_table


class InfoObjClass(gdb.Command):
    \"\"\"show object class hierarchy or list all classes\"\"\"

    def __init__(self):
        super(InfoObjClass, self).__init__(
            "info obj_class", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Show object class info.")
        parser.add_argument(
            "--all",
            action="store_true",
            default=False,
            help="List all registered object classes.",
        )
        parser.add_argument(
            "expr",
            type=str,
            nargs="?",
            default=None,
            help="Expression evaluating to an lv_obj_class_t.",
        )
        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.all or not args.expr:
            classes = LVObjClass.collect_all()
            snaps = LVObjClass.snapshots(classes)
            if snaps:
                print_spec_table(snaps)
            else:
                print(LVObjClass._DISPLAY_SPEC["empty_msg"])
            return

        try:
            cls = LVObjClass(args.expr)
        except gdb.error as e:
            print(f"Error: {e}")
            return
        print_info(cls.snapshot())
"""

FILES['lvglgdb/cmds/core/lv_observer.py'] = """import gdb

from lvglgdb.lvgl.core.lv_observer import LVSubject
from lvglgdb.lvgl.formatter import print_info


class InfoSubject(gdb.Command):
    \"\"\"show subject and its observers\"\"\"

    def __init__(self):
        super(InfoSubject, self).__init__(
            "info subject", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        if not args.strip():
            print("Usage: info subject <expression>")
            return
        try:
            subject = LVSubject(args.strip())
        except gdb.error as e:
            print(f"Error: {e}")
            return
        print_info(subject.snapshot())
"""

FILES['lvglgdb/cmds/core/lv_widget.py'] = """\"\"\"`info widget`: everything known about a widget, in a readable list.

`dump obj` walks the tree but prints one line per widget, and `info obj_class`
answers about the class, not the instance in front of you. Between them the
per-widget fields the wrappers collect - a label's text, an arc's angles - were
only reachable through `dump dashboard`. This command prints the same data the
dashboard shows, for one widget or for every widget on every display - and,
given field names, only the fields asked for.

`dump widget props` answers the question that comes right after: which field
names does this widget have? It reads them from the generated wrapper, so a
class name is enough - no live object needed.
\"\"\"

import argparse

import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_obj import LVObject
from lvglgdb.lvgl.misc.lv_style import read_symbol
from lvglgdb.lvgl.misc.lv_utils import format_enum
from lvglgdb.lvgl.widgets import specs
from lvglgdb.value import CorruptedError

_LABEL_W = 14


# --- finding a widget by name ------------------------------------------------
#
# The names are resolved the way lv_obj_get_name_resolved() resolves them, and
# the search follows lv_obj_find_by_name() / lv_obj_get_child_by_name(): a
# trailing '#' is an auto-index over the siblings sharing that name, and a
# widget that was never named answers to `<class>_<n>`. Reimplementing this
# rather than calling into the target keeps it working on a core dump, where
# nothing can be called.


def resolved_name(obj) -> str:
    \"\"\"The name `lv_obj_get_name_resolved()` would produce for this widget.\"\"\"
    raw = obj._get_name()
    auto = raw is None
    name = f"{obj.class_name}_#" if auto else raw

    if not name.endswith("#"):
        return name

    parent = obj.super_value("parent")
    parent = LVObject(parent) if parent.is_ok and int(parent) else None
    # A screen has no parent, so its siblings are the other screens of its
    # display. LVGL leaves the '#' unresolved there; indexing it as well is what
    # makes a screen the typable first segment of a name path.
    siblings = parent.children if parent is not None else _sibling_screens(obj)
    if siblings is None:
        return name

    index = 0
    for sibling in siblings:
        if int(sibling) == int(obj):
            return name[:-1] + str(index)
        sibling_name = sibling._get_name()
        if sibling_name is None:
            # An unnamed sibling counts when the name we resolve is its own
            # auto name, i.e. `<its class>_#`.
            class_name = sibling.class_name
            if len(name) > 3 and len(class_name) == len(name) - 2 \\
                    and name.startswith(class_name):
                index += 1
        elif sibling_name == name:
            index += 1
    return name


def _sibling_screens(obj):
    \"\"\"The screen list holding this screen, or None if it is not a screen.\"\"\"
    if not curr_inst().ensure_init():
        return None
    for disp in curr_inst().displays():
        try:
            found = list(disp.screens)
        except CorruptedError:
            continue
        if any(int(s) == int(obj) for s in found):
            return found
    return None


def name_path(obj) -> str:
    \"\"\"The resolved names from the screen down to this widget, slash separated.\"\"\"
    parts = []
    node = obj
    seen = set()
    while node is not None and int(node) not in seen:
        seen.add(int(node))
        try:
            parts.append(resolved_name(node))
            parent = node.super_value("parent")
        except CorruptedError:
            parts.append("?")
            break
        node = LVObject(parent) if parent.is_ok and int(parent) else None
    return "/".join(reversed(parts))


def screens():
    \"\"\"Every screen of every display, the roots a name path starts from.\"\"\"
    if not curr_inst().ensure_init():
        return []
    result = []
    for disp in curr_inst().displays():
        try:
            result.extend(disp.screens)
        except CorruptedError:
            continue
    return result


def find_by_name(name, parent=None):
    \"\"\"Widgets called *name* below *parent*, or below every screen.

    Unlike `lv_obj_find_by_name()`, which stops at the first hit on the active
    screen, every match is collected: in the debugger an ambiguous name is
    worth seeing rather than silently resolving to one of several widgets.
    \"\"\"
    found = []

    def walk(obj, matchable=True):
        try:
            if matchable and _name_is(obj, name):
                found.append(obj)
            for child in obj.children:
                walk(child)
        except CorruptedError:
            pass

    if parent is not None:
        # Like lv_obj_find_by_name(), the search starts below the parent: with
        # `info widget dialog -p dialog` the parent itself is not the answer.
        walk(parent, matchable=False)
    else:
        for root in screens():
            walk(root)
    return found


def find_by_path(path, parent=None):
    \"\"\"The widget at a slash separated name path, or None.

    With no *parent* the first segment names a screen, so
    `screen_1/button_2/label3` is a whole path from the top.
    \"\"\"
    segments = [seg for seg in path.split("/") if seg]
    if not segments:
        return None

    if parent is None:
        candidates = [s for s in screens() if _name_is(s, segments[0])]
        segments = segments[1:]
    else:
        candidates = [parent]

    for segment in segments:
        nxt = None
        for candidate in candidates:
            try:
                for child in candidate.children:
                    if _name_is(child, segment):
                        nxt = child
                        break
            except CorruptedError:
                continue
            if nxt is not None:
                break
        if nxt is None:
            return None
        candidates = [nxt]

    return candidates[0] if candidates else None


def _name_is(obj, name) -> bool:
    \"\"\"A widget answers to its resolved name and to the raw one it was given.\"\"\"
    try:
        return name in (resolved_name(obj), obj._get_name())
    except CorruptedError:
        return False


def resolve_widget(token, parent_path=None):
    \"\"\"Turn what the user typed into one widget, or explain why it is not one.

    Returns (widget, error). The token is a C expression when GDB can evaluate
    it - a variable, a member chain, an address - and a name or a name path
    otherwise.
    \"\"\"
    parent = None
    if parent_path:
        parent, error = resolve_widget(parent_path)
        if parent is None:
            return None, f"--parent: {error}"

    if "/" in token:
        found = find_by_path(token, parent)
        if found is None:
            where = f" under {parent_path}" if parent_path else ""
            return None, f"no widget at path '{token}'{where}"
        return found, None

    if parent is None:
        try:
            return LVObject(gdb.parse_and_eval(token)), None
        except gdb.error:
            pass  # not an expression, so it is a name

    matches = find_by_name(token, parent)
    if not matches:
        where = f" under {parent_path}" if parent_path else ""
        return None, f"no widget named '{token}'{where}"
    if len(matches) > 1:
        lines = [f"'{token}' matches {len(matches)} widgets:"]
        lines += [
            f"  {name_path(m)}  ({m.class_name} @{hex(int(m))})" for m in matches
        ]
        lines.append("Narrow it down with --parent, or pass a name path.")
        return None, "\\n".join(lines)
    return matches[0], None


def _snapshot(obj) -> dict:
    \"\"\"Snapshot of a single widget, widget_data included, children excluded.\"\"\"
    return LVObject._wrap_as_widget(obj).snapshot().as_dict()


def _field(label, value, indent=1):
    print(f"{'  ' * indent}{label:{_LABEL_W}s} = {value}")


def _addr_str(addr) -> str:
    \"\"\"A pointer with its C symbol, when the address has one.\"\"\"
    if not addr:
        return "-"
    try:
        symbol = read_symbol(int(addr, 16))
    except (ValueError, TypeError):
        symbol = None
    return f"{addr} <{symbol}>" if symbol else str(addr)


def _size_str(d) -> str:
    c = d.get("coords") or {}
    x1, y1 = c.get("x1", 0), c.get("y1", 0)
    x2, y2 = c.get("x2", 0), c.get("y2", 0)
    return f"({x1},{y1})-({x2},{y2})  {x2 - x1 + 1}x{y2 - y1 + 1}"


def _headline(obj, d) -> str:
    gist = specs.summary(d.get("class_name", ""), d.get("widget_data"))
    parts = [f"{d.get('class_name', '?')} @{d.get('addr', '?')}"]
    try:
        parts.append(f"name={resolved_name(obj)}")
    except CorruptedError:
        pass
    if gist:
        parts.append(f'"{gist}"' if not gist.startswith('"') else gist)
    return "  ".join(parts)


def _compact(obj) -> str:
    \"\"\"One line for the tree listing.\"\"\"
    d = _snapshot(obj)
    line = _headline(obj, d)
    c = d.get("coords") or {}
    x1, y1 = c.get("x1", 0), c.get("y1", 0)
    line += f"  ({x1},{y1}) {c.get('x2', 0) - x1 + 1}x{c.get('y2', 0) - y1 + 1}"
    states = [s for s in d.get("state_list", []) if s != "DEFAULT"]
    if states:
        line += f"  state={'|'.join(states)}"
    if d.get("error"):
        line += f"  (corrupted: {d['error']})"
    return line


def _print_scroll(d):
    scroll = d.get("scroll")
    dir_raw = d.get("scroll_dir")
    if scroll is None and dir_raw is None:
        return
    parts = []
    if scroll:
        parts.append(f"({scroll['x']},{scroll['y']})")
    if dir_raw is not None:
        parts.append(
            f"dir={format_enum('lv_dir_t', dir_raw, 'LV_DIR_', bitmask=True)}"
        )
    snap_x, snap_y = d.get("scroll_snap_x"), d.get("scroll_snap_y")
    if snap_x is not None or snap_y is not None:
        sx = format_enum("lv_scroll_snap_t", snap_x or 0, "LV_SCROLL_SNAP_")
        sy = format_enum("lv_scroll_snap_t", snap_y or 0, "LV_SCROLL_SNAP_")
        parts.append(f"snap=({sx},{sy})")
    mode = d.get("scrollbar_mode")
    if mode is not None:
        parts.append(
            f"bar={format_enum('lv_scrollbar_mode_t', mode, 'LV_SCROLLBAR_MODE_')}"
        )
    _field("scroll", "  ".join(parts))


def _print_events(obj):
    \"\"\"Event callbacks, with the symbol GDB resolves each one to.\"\"\"
    try:
        events = obj.event_list
        dscs = list(events) if events else []
    except (gdb.error, CorruptedError):
        return
    if not dscs:
        return
    _field("events", len(dscs))
    for i, dsc in enumerate(dscs):
        e = dsc.snapshot().as_dict()
        print(
            f"    [{i}] {e['filter_name']:<18s} cb={e['cb']}"
            f"  user_data={e['user_data']}  flags={e['flags_str']}"
        )


def _print_widget_data(d):
    \"\"\"The widget's own struct fields, primary ones first.\"\"\"
    class_name = d.get("class_name", "")
    data = d.get("widget_data") or {}
    if not data:
        return
    print(f"  {class_name}")
    for key in specs.order_fields(class_name, data.keys()):
        print(
            f"    {key:{_LABEL_W}s} = "
            f"{specs.format_value(class_name, key, data[key])}"
        )


def _obj_field(d, name):
    \"\"\"An object-level field by name, accepting a few friendly aliases.\"\"\"
    aliases = {
        "class": "class_name",
        "state": "state_list",
        "flags": "flags_list",
        "parent": "parent_addr",
        "group": "group_addr",
        "children": "child_count",
        "styles": "style_count",
    }
    key = aliases.get(name, name)
    if key not in d or key == "widget_data":
        return None, False
    return d[key], True


def _format_obj_field(key, value):
    \"\"\"Render an object-level field the way the full listing renders it.\"\"\"
    if key in ("state", "state_list"):
        return "|".join(value or ["DEFAULT"])
    if key in ("flags", "flags_list"):
        return "|".join(value or []) or "-"
    if key == "coords":
        return _size_str({"coords": value})
    if key in ("parent", "parent_addr", "group", "group_addr", "user_data"):
        return _addr_str(value)
    if key == "scroll_dir":
        return format_enum("lv_dir_t", value, "LV_DIR_", bitmask=True)
    if key in ("scroll_snap_x", "scroll_snap_y"):
        return format_enum("lv_scroll_snap_t", value, "LV_SCROLL_SNAP_")
    if key == "scrollbar_mode":
        return format_enum("lv_scrollbar_mode_t", value, "LV_SCROLLBAR_MODE_")
    if key == "layer_type":
        return format_enum("lv_layer_type_t", value, "LV_LAYER_TYPE_")
    return "-" if value is None else str(value)


def print_widget_fields(obj, names):
    \"\"\"Print only the named fields of one widget.\"\"\"
    d = _snapshot(obj)
    class_name = d.get("class_name", "")
    data = d.get("widget_data") or {}
    for name in names:
        if name in data:
            _field(name, specs.format_value(class_name, name, data[name]), indent=0)
            continue
        value, found = _obj_field(d, name)
        if found:
            _field(name, _format_obj_field(name, value), indent=0)
            continue
        print(f"{name}: no such field on {class_name}"
              f" (see: dump widget props {class_name})")


def print_widget(obj, expr=None):
    \"\"\"Print everything known about one widget.\"\"\"
    d = _snapshot(obj)

    print(_headline(obj, d))
    chain = []
    cls = obj.obj_class
    if cls:
        chain = [c.name for c in cls]
    _field("class", " -> ".join(chain) if chain else d.get("class_name", "?"))
    _field("parent", _addr_str(d.get("parent_addr")))
    _field("coords", _size_str(d))
    _field("children", d.get("child_count", 0))
    _field("state", "|".join(d.get("state_list") or ["DEFAULT"]))
    _field("flags", "|".join(d.get("flags_list") or []) or "-")
    _print_scroll(d)
    if d.get("ext_click_pad") is not None or d.get("ext_draw_size") is not None:
        _field(
            "ext_pad",
            f"click={d.get('ext_click_pad', 0)}  draw={d.get('ext_draw_size', 0)}",
        )
    layer = d.get("layer_type")
    _field(
        "layout",
        f"inv={d.get('layout_inv')}  w={d.get('w_layout')}  h={d.get('h_layout')}"
        + (
            f"  layer={format_enum('lv_layer_type_t', layer, 'LV_LAYER_TYPE_')}"
            if layer is not None
            else ""
        ),
    )
    _field(
        "status",
        f"rendered={d.get('rendered')}  deleting={d.get('is_deleting')}"
        f"  skip_trans={d.get('skip_trans')}",
    )
    if d.get("group_addr"):
        _field("group", d["group_addr"])
    if d.get("user_data"):
        _field("user_data", _addr_str(d["user_data"]))

    style_cnt = d.get("style_count", 0)
    hint = f"   (info style --obj {expr})" if expr and style_cnt else ""
    _field("styles", f"{style_cnt}{hint}")

    _print_events(obj)
    _print_widget_data(d)

    try:
        children = list(obj.children)
    except CorruptedError:
        print("  children       = (corrupted)")
        return
    if children:
        print("  children")
        for i, child in enumerate(children):
            try:
                print(f"    [{i}] {_compact(child)}")
            except CorruptedError as e:
                print(f"    [{i}] (corrupted: {e})")


def print_tree(obj, depth=0, limit=None):
    \"\"\"Print one compact line per widget, indented by tree depth.\"\"\"
    try:
        print("  " * depth + _compact(obj))
    except CorruptedError as e:
        print("  " * depth + f"(corrupted: {e})")
        return
    if limit is not None and depth >= limit:
        return
    try:
        for child in obj.children:
            print_tree(child, depth + 1, limit=limit)
    except CorruptedError:
        print("  " * (depth + 1) + "(corrupted children)")


def print_all_widgets(limit=None):
    \"\"\"List every widget of every display, screens labelled with their layer.\"\"\"
    if not curr_inst().ensure_init():
        return
    for disp in curr_inst().displays():
        print(f"Display @{hex(int(disp))}")
        layers = disp.layer_addrs
        try:
            for screen in disp.screens:
                layer = layers.get(int(screen))
                suffix = f" ({layer})" if layer else ""
                print(f"  Screen @{hex(int(screen))}{suffix}")
                print_tree(screen, depth=2, limit=limit)
        except CorruptedError:
            print("  (corrupted screens)")


class InfoWidget(gdb.Command):
    \"\"\"show all details of a widget, or list every widget\"\"\"

    def __init__(self):
        super(InfoWidget, self).__init__(
            "info widget", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(
            prog="info widget",
            description="Show widget details, or list all widgets.",
        )
        parser.add_argument(
            "-t",
            "--tree",
            action="store_true",
            help="List the subtree of the widget instead of its details.",
        )
        parser.add_argument(
            "-L",
            "--level",
            type=int,
            default=None,
            help="Limit the depth of a listing.",
        )
        parser.add_argument(
            "-p",
            "--parent",
            type=str,
            default=None,
            help="Search for the name below this widget, e.g. screen_1/button_2.",
        )
        parser.add_argument(
            "widget_and_fields",
            metavar="widget [field...]",
            type=str,
            nargs="*",
            help="The widget - a name, a name path, an expression or an address"
                 " - then the field names to print instead of the whole widget.",
        )
        try:
            # parse_known_args, because argparse fills a positional list in one
            # go: in `info widget label3 -p screen_1 text` everything after the
            # option would be dropped as unrecognised. The leftovers are the
            # rest of the positionals, in the order they were typed.
            opts, rest = parser.parse_known_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        flags = [word for word in rest if word.startswith("-")]
        if flags:
            print(f"Error: unknown option {flags[0]}")
            return

        positional = opts.widget_and_fields + rest
        widget = positional[0] if positional else None
        opts.fields = positional[1:]

        if opts.parent and not widget:
            print("Error: --parent needs a name to look for.")
            return

        if widget:
            obj, error = resolve_widget(widget, opts.parent)
            if obj is None:
                print(f"Error: {error}")
                return
            if opts.tree:
                print_tree(obj, limit=opts.level)
            elif opts.fields:
                print_widget_fields(obj, opts.fields)
            else:
                print_widget(obj, expr=widget)
            return

        print_all_widgets(limit=opts.level)


_COMMON_FIELDS = (
    "name", "class_name", "addr", "coords", "parent", "children", "styles",
    "state", "flags", "scroll", "scroll_dir", "scroll_snap_x", "scroll_snap_y",
    "scrollbar_mode", "ext_click_pad", "ext_draw_size", "layer_type",
    "layout_inv", "w_layout", "h_layout", "rendered", "is_deleting",
    "skip_trans", "group", "user_data",
)


def _wrapper_class(class_name):
    \"\"\"The generated wrapper for a class name, or None.\"\"\"
    from lvglgdb.lvgl.widgets import WIDGET_REGISTRY

    return WIDGET_REGISTRY.get(class_name) or WIDGET_REGISTRY.get("lv_" + class_name)


def _class_name_of(cls):
    \"\"\"'lv_bar' for the wrapper defined in widgets/lv_bar.py.\"\"\"
    return cls.__module__.rsplit(".", 1)[-1]


def print_widget_props(class_name, live_fields=None):
    \"\"\"List the field names a widget has, grouped by the struct they come from.\"\"\"
    cls = _wrapper_class(class_name)
    if cls is None:
        print(f"{class_name}  (no fields of its own)")
    else:
        print(f"{_class_name_of(cls)}  ({cls.__name__})")

    # Only the generated wrappers, so that LVObject's and Value's own helper
    # properties - x1, obj_styles, is_ok - stay out of a list of widget fields.
    for owner in (cls.__mro__ if cls else ()):
        if not owner.__module__.startswith("lvglgdb.lvgl.widgets"):
            continue
        props = [
            (name, value)
            for name, value in vars(owner).items()
            if isinstance(value, property)
        ]
        if not props:
            continue
        owner_name = _class_name_of(owner)
        types = specs.field_types(owner_name)
        print(f"  from {owner_name}_t")
        for name, prop in props:
            doc = (prop.__doc__ or "").strip().splitlines()
            desc = doc[0] if doc else ""
            print(f"    {name:{_LABEL_W + 4}s} {types.get(name, ''):<8s} {desc}".rstrip())

    print("  common (any widget)")
    fields = live_fields if live_fields is not None else _COMMON_FIELDS
    line = "    "
    for name in fields:
        if len(line) + len(name) > 76:
            print(line)
            line = "    "
        line += name + ", "
    print(line.rstrip(", "))


def _friendly_names(keys):
    \"\"\"Snapshot keys as the names to type: the aliases _obj_field() accepts.

    A snapshot carries both `flags` and `flags_list`; only one of them is worth
    listing as a field name, and it is the short one.
    \"\"\"
    renamed = {
        "parent_addr": "parent",
        "group_addr": "group",
        "child_count": "children",
        "style_count": "styles",
    }
    dropped = ("flags_list", "state_list")
    return [renamed.get(k, k) for k in keys if k not in dropped]


def print_widget_classes():
    from lvglgdb.lvgl.widgets import WIDGET_REGISTRY

    print("Widget classes with fields of their own:")
    for name in sorted(WIDGET_REGISTRY):
        print(f"  {name}")


class DumpWidget(gdb.Command):
    \"\"\"widget related dumps\"\"\"

    def __init__(self):
        super(DumpWidget, self).__init__(
            "dump widget", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION, prefix=True
        )


class DumpWidgetProps(gdb.Command):
    \"\"\"list the field names of a widget class or of a live widget\"\"\"

    def __init__(self):
        super(DumpWidgetProps, self).__init__(
            "dump widget props", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(
            prog="dump widget props",
            description="List the fields 'info widget' can print.",
        )
        parser.add_argument(
            "target",
            type=str,
            nargs="?",
            default=None,
            help="A widget class name (lv_label), or a widget itself:"
                 " a variable, a member chain, or its address.",
        )
        try:
            opts = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if not opts.target:
            print_widget_classes()
            return

        if _wrapper_class(opts.target) is not None:
            print_widget_props(opts.target)
            return

        obj, error = resolve_widget(opts.target)
        if obj is None:
            print(f"'{opts.target}' is neither a widget class nor a widget.")
            print(f"({error})")
            return
        try:
            d = _snapshot(obj)
        except CorruptedError as e:
            print(f"Error: {e}")
            return
        live = _friendly_names(
            k for k in d if k not in ("widget_data", "children", "styles")
        )
        print_widget_props(d.get("class_name", ""), live_fields=live)
"""

FILES['lvglgdb/cmds/dashboard/__init__.py'] = """from .lv_dashboard import DumpDashboard

__all__ = ["DumpDashboard"]
"""

FILES['lvglgdb/cmds/dashboard/data_collector.py'] = """import base64
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
    \"\"\"Collect a simple subsystem using the registry entry.

    For entries with accessor != None: [x.snapshot().as_dict() for x in lvgl.<accessor>()]
    For entries with accessor == None: [s.as_dict() for s in lvgl.<dict_key>().snapshots()]
    \"\"\"

    @safe_collect(label)
    def _inner():
        if accessor is not None:
            return [x.snapshot().as_dict() for x in getattr(lvgl, accessor)()]
        return [s.as_dict() for s in getattr(lvgl, dict_key)().snapshots()]

    return _inner()


def collect_all() -> dict:
    \"\"\"Collect all LVGL runtime data into a JSON-compatible dict.\"\"\"
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
    # Widget specs (global lookup table for uinspy rendering)
    data["widget_specs"] = _collect_widget_specs(data.get("object_trees", []))
    return data


def _get_lvgl_version() -> str | None:
    \"\"\"Try to read LVGL version from macros.\"\"\"
    try:
        major = int(gdb.parse_and_eval("LVGL_VERSION_MAJOR"))
        minor = int(gdb.parse_and_eval("LVGL_VERSION_MINOR"))
        patch = int(gdb.parse_and_eval("LVGL_VERSION_PATCH"))
        return f"{major}.{minor}.{patch}"
    except gdb.error:
        return None


def _buf_to_dict(draw_buf) -> dict | None:
    \"\"\"Convert an LVDrawBuf to a dict with base64 PNG image.\"\"\"
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
    \"\"\"Collect display info with framebuffer data.\"\"\"
    result = []
    for disp in lvgl.displays():
        d = disp.snapshot().as_dict()
        d["buf_1"] = _buf_to_dict(disp.buf_1)
        d["buf_2"] = _buf_to_dict(disp.buf_2)
        result.append(d)
    return result


@safe_collect("object trees")
def _collect_object_trees(lvgl) -> list:
    \"\"\"Collect object trees for all displays with layer name annotations.\"\"\"
    from lvglgdb.lvgl.snapshot import Snapshot

    result = []
    for disp in lvgl.displays():
        addrs = disp.layer_addrs

        @Snapshot.fallback(layer_name=lambda s: addrs.get(int(s)))
        def _screen_snapshot(screen):
            from lvglgdb.lvgl.core.lv_obj import LVObject
            w = LVObject._wrap_as_widget(screen)
            snap = w.snapshot(
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
    \"\"\"Collect image cache entries with optional decoded buffer previews.\"\"\"
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
    \"\"\"Collect draw tasks from each display's layer chain.\"\"\"
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
    \"\"\"Collect subjects from object event lists across all displays.\"\"\"
    seen = set()
    result = []
    for disp in lvgl.displays():
        for screen in disp.screens:
            _collect_subjects_from_obj(screen, seen, result)
    return result


def _collect_subjects_from_obj(obj, seen, result):
    \"\"\"Recursively collect subjects from an object's event list.\"\"\"
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


def _collect_widget_specs(object_trees: list) -> dict:
    \"\"\"Build widget_specs dict for all class_names found in object trees.\"\"\"
    import json as _json
    from pathlib import Path

    specs_path = Path(__file__).parent.parent.parent.parent / "scripts" / "generators" / "widget_specs.json"
    if not specs_path.exists():
        return {}

    try:
        all_specs = _json.loads(specs_path.read_text())
    except Exception:
        return {}

    # Collect all class_names from object trees
    seen = set()

    def _walk(node):
        cn = node.get("class_name", "")
        if cn:
            seen.add(cn)
        for child in node.get("children", []):
            _walk(child)

    for tree in object_trees:
        for screen in tree.get("screens", []):
            _walk(screen)

    # Build output: merge _auto + enums into flat fields spec
    result = {}
    for cn in seen:
        raw = all_specs.get(cn) or all_specs.get("lv_" + cn)
        if not raw:
            continue
        auto_fields = raw.get("_auto", {}).get("fields", {})
        enums = raw.get("enums", {})
        fields = {}
        for fname, ftype in auto_fields.items():
            entry = {"type": ftype}
            if fname in enums:
                entry["type"] = "enum"
                entry["names"] = enums[fname]
            fields[fname] = entry

        spec = {}
        if raw.get("summary_tpl") is not None:
            spec["summary_tpl"] = raw["summary_tpl"]
        if raw.get("primary"):
            spec["primary"] = raw["primary"]
        if fields:
            spec["fields"] = fields
        if spec:
            result[cn] = spec
    return result
"""

FILES['lvglgdb/cmds/dashboard/html_renderer.py'] = """import json
from pathlib import Path

_STATIC_DIR = Path(__file__).parent / "static"


def render(data: dict, output_path: str) -> None:
    \"\"\"Generate self-contained HTML with JSON data embedded.\"\"\"
    json_str = _safe_json_encode(data)
    html = _build_html(json_str)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)


def render_viewer(output_path: str) -> None:
    \"\"\"Generate empty shell HTML viewer (no embedded data).\"\"\"
    html = _build_html("")
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)


def _safe_json_encode(data: dict) -> str:
    \"\"\"Serialize dict to JSON with HTML-safe escaping.\"\"\"
    raw = json.dumps(data, ensure_ascii=False, indent=None)
    # Escape HTML-special chars to prevent injection in <script> block
    return raw.replace("&", "\\\\u0026").replace("<", "\\\\u003c").replace(">", "\\\\u003e")


def _read_static(filename: str) -> str:
    \"\"\"Read a static asset file from the static/ directory.\"\"\"
    return (_STATIC_DIR / filename).read_text(encoding="utf-8")


def _build_html(json_content: str) -> str:
    \"\"\"Build the complete HTML page with embedded or empty JSON.\"\"\"
    html = _read_static("uinspy.html")
    return html.replace("{{JSON_DATA}}", json_content, 1)
"""

FILES['lvglgdb/cmds/dashboard/lv_dashboard.py'] = """import argparse
import json
import time

import gdb

from lvglgdb.lvgl import curr_inst
from .data_collector import collect_all
from .html_renderer import render, render_viewer


class DumpDashboard(gdb.Command):
    \"\"\"Generate an HTML dashboard of all LVGL runtime state.\"\"\"

    def __init__(self):
        super().__init__(
            "dump dashboard", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(prog="dump dashboard")
        parser.add_argument("-o", "--output", help="output file path")
        group = parser.add_mutually_exclusive_group()
        group.add_argument(
            "--json", action="store_true", help="output JSON instead of HTML",
        )
        group.add_argument(
            "--viewer", action="store_true",
            help="output empty viewer HTML (no data collection)",
        )

        try:
            opts = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if opts.viewer:
            out = opts.output or "lvgl_viewer.html"
            render_viewer(out)
            gdb.write(f"Viewer written to {out}\\n")
            return

        if not curr_inst().ensure_init():
            return

        t0 = time.time()
        data = collect_all()
        elapsed = time.time() - t0

        if opts.json:
            out = opts.output or "lvgl_dashboard.json"
            with open(out, "w", encoding="utf-8") as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        else:
            out = opts.output or "lvgl_dashboard.html"
            render(data, out)

        gdb.write(f"Dashboard written to {out} ({elapsed:.2f}s)\\n")
"""

FILES['lvglgdb/cmds/dashboard/static/uinspy.html'] = """<!doctype html>
<html lang="en">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1.0"/>
<title>LVGL Dashboard</title>
<link rel="icon" href="data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzMiAzMiI+CiAgPHJlY3Qgd2lkdGg9IjMyIiBoZWlnaHQ9IjMyIiByeD0iNiIgZmlsbD0iIzdjM2FlZCIvPgogIDxwYXRoIGQ9Ik05IDExaDZ2Mkg5em0wIDRoMTB2Mkg5em0wIDRoOHYySDl6bTE2LThsLTQgNiA0IDZoLTNsLTQtNiA0LTZ6IiBmaWxsPSIjZmZmIi8+Cjwvc3ZnPgo="/>
<style>/*! tailwindcss v4.2.1 | MIT License | https://tailwindcss.com */
@layer properties;
@layer theme, base, components, utilities;
@layer theme {
  :root, :host {
    --font-sans: ui-sans-serif, system-ui, sans-serif, "Apple Color Emoji",
      "Segoe UI Emoji", "Segoe UI Symbol", "Noto Color Emoji";
    --font-mono: 'JetBrains Mono', 'Fira Code', monospace;
    --spacing: 0.25rem;
    --text-xs: 0.75rem;
    --text-xs--line-height: calc(1 / 0.75);
    --text-sm: 0.875rem;
    --text-sm--line-height: calc(1.25 / 0.875);
    --text-lg: 1.125rem;
    --text-lg--line-height: calc(1.75 / 1.125);
    --text-2xl: 1.5rem;
    --text-2xl--line-height: calc(2 / 1.5);
    --text-4xl: 2.25rem;
    --text-4xl--line-height: calc(2.5 / 2.25);
    --font-weight-medium: 500;
    --font-weight-semibold: 600;
    --font-weight-bold: 700;
    --font-weight-extrabold: 800;
    --leading-tight: 1.25;
    --radius-sm: 0.25rem;
    --radius-md: 0.375rem;
    --radius-lg: 0.5rem;
    --radius-xl: 0.75rem;
    --default-font-family: var(--font-sans);
    --default-mono-font-family: var(--font-mono);
    --color-crust: var(--crust);
    --color-mantle: var(--mantle);
    --color-base: var(--base);
    --color-surface0: var(--surface0);
    --color-overlay0: var(--overlay0);
    --color-overlay1: var(--overlay1);
    --color-txt: var(--text);
    --color-subtext0: var(--subtext0);
    --color-subtext1: var(--subtext1);
    --color-blue: var(--blue);
    --color-sapphire: var(--sapphire);
    --color-green: var(--green);
    --color-yellow: var(--yellow);
    --color-peach: var(--peach);
    --color-mauve: var(--mauve);
    --color-teal: var(--teal);
    --color-panel-bg: var(--panel-bg);
    --color-hover-row: var(--hover-row);
    --color-hover-summary: var(--hover-summary);
    --color-glow-blue: var(--glow-blue);
    --color-nav-active-bg: var(--nav-active-bg);
    --color-version-bg: var(--version-bg);
    --color-icon-bg-blue: var(--icon-bg-blue);
    --color-icon-bg-green: var(--icon-bg-green);
    --color-icon-bg-mauve: var(--icon-bg-mauve);
    --color-icon-bg-peach: var(--icon-bg-peach);
    --color-icon-bg-teal: var(--icon-bg-teal);
    --color-icon-bg-pink: var(--icon-bg-pink);
  }
}
@layer base {
  *, ::after, ::before, ::backdrop, ::file-selector-button {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
    border: 0 solid;
  }
  html, :host {
    line-height: 1.5;
    -webkit-text-size-adjust: 100%;
    tab-size: 4;
    font-family: var(--default-font-family, ui-sans-serif, system-ui, sans-serif, "Apple Color Emoji", "Segoe UI Emoji", "Segoe UI Symbol", "Noto Color Emoji");
    font-feature-settings: var(--default-font-feature-settings, normal);
    font-variation-settings: var(--default-font-variation-settings, normal);
    -webkit-tap-highlight-color: transparent;
  }
  hr {
    height: 0;
    color: inherit;
    border-top-width: 1px;
  }
  abbr:where([title]) {
    -webkit-text-decoration: underline dotted;
    text-decoration: underline dotted;
  }
  h1, h2, h3, h4, h5, h6 {
    font-size: inherit;
    font-weight: inherit;
  }
  a {
    color: inherit;
    -webkit-text-decoration: inherit;
    text-decoration: inherit;
  }
  b, strong {
    font-weight: bolder;
  }
  code, kbd, samp, pre {
    font-family: var(--default-mono-font-family, ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace);
    font-feature-settings: var(--default-mono-font-feature-settings, normal);
    font-variation-settings: var(--default-mono-font-variation-settings, normal);
    font-size: 1em;
  }
  small {
    font-size: 80%;
  }
  sub, sup {
    font-size: 75%;
    line-height: 0;
    position: relative;
    vertical-align: baseline;
  }
  sub {
    bottom: -0.25em;
  }
  sup {
    top: -0.5em;
  }
  table {
    text-indent: 0;
    border-color: inherit;
    border-collapse: collapse;
  }
  :-moz-focusring {
    outline: auto;
  }
  progress {
    vertical-align: baseline;
  }
  summary {
    display: list-item;
  }
  ol, ul, menu {
    list-style: none;
  }
  img, svg, video, canvas, audio, iframe, embed, object {
    display: block;
    vertical-align: middle;
  }
  img, video {
    max-width: 100%;
    height: auto;
  }
  button, input, select, optgroup, textarea, ::file-selector-button {
    font: inherit;
    font-feature-settings: inherit;
    font-variation-settings: inherit;
    letter-spacing: inherit;
    color: inherit;
    border-radius: 0;
    background-color: transparent;
    opacity: 1;
  }
  :where(select:is([multiple], [size])) optgroup {
    font-weight: bolder;
  }
  :where(select:is([multiple], [size])) optgroup option {
    padding-inline-start: 20px;
  }
  ::file-selector-button {
    margin-inline-end: 4px;
  }
  ::placeholder {
    opacity: 1;
  }
  @supports (not (-webkit-appearance: -apple-pay-button))  or (contain-intrinsic-size: 1px) {
    ::placeholder {
      color: currentcolor;
      @supports (color: color-mix(in lab, red, red)) {
        color: color-mix(in oklab, currentcolor 50%, transparent);
      }
    }
  }
  textarea {
    resize: vertical;
  }
  ::-webkit-search-decoration {
    -webkit-appearance: none;
  }
  ::-webkit-date-and-time-value {
    min-height: 1lh;
    text-align: inherit;
  }
  ::-webkit-datetime-edit {
    display: inline-flex;
  }
  ::-webkit-datetime-edit-fields-wrapper {
    padding: 0;
  }
  ::-webkit-datetime-edit, ::-webkit-datetime-edit-year-field, ::-webkit-datetime-edit-month-field, ::-webkit-datetime-edit-day-field, ::-webkit-datetime-edit-hour-field, ::-webkit-datetime-edit-minute-field, ::-webkit-datetime-edit-second-field, ::-webkit-datetime-edit-millisecond-field, ::-webkit-datetime-edit-meridiem-field {
    padding-block: 0;
  }
  ::-webkit-calendar-picker-indicator {
    line-height: 1;
  }
  :-moz-ui-invalid {
    box-shadow: none;
  }
  button, input:where([type="button"], [type="reset"], [type="submit"]), ::file-selector-button {
    appearance: button;
  }
  ::-webkit-inner-spin-button, ::-webkit-outer-spin-button {
    height: auto;
  }
  [hidden]:where(:not([hidden="until-found"])) {
    display: none !important;
  }
}
@layer utilities;
@layer base {
  * {
    margin: calc(var(--spacing) * 0);
    box-sizing: border-box;
    padding: calc(var(--spacing) * 0);
  }
  body {
    display: flex;
    min-height: 100vh;
    flex-direction: column;
    background-color: var(--color-crust);
    color: var(--color-txt);
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    font-family: Inter, -apple-system, BlinkMacSystemFont, 'Segoe UI', system-ui, Roboto, sans-serif;
    font-size: 13px;
    line-height: 1.5;
    transition: background .3s, color .3s;
  }
  body::after {
    content: var(--body-after-content);
    position: fixed;
    inset: 0;
    pointer-events: none;
    z-index: 9999;
    background: var(--body-after-bg);
    opacity: .5;
  }
}
@layer components {
  .panel {
    display: flex;
    flex-direction: column;
    overflow: hidden;
    background-color: var(--color-panel-bg);
    border: 1px solid var(--panel-border);
    border-radius: var(--ui-radius);
    box-shadow: var(--panel-glow);
    transition: border-color var(--transition), box-shadow var(--transition), background var(--transition);
  }
  .panel:hover {
    border-color: var(--surface1);
    box-shadow: 0 0 0 1px var(--surface1), var(--shadow-hover), var(--panel-hover-glow);
  }
  .panel-header {
    display: flex;
    flex-shrink: 0;
    align-items: center;
    gap: calc(var(--spacing) * 2);
    padding-inline: calc(var(--spacing) * 3);
    padding-block: calc(var(--spacing) * 2);
    border-bottom: 1px solid var(--panel-border);
    background: var(--panel-header-bg), var(--panel-header-gradient);
  }
  .panel-icon {
    flex-shrink: 0;
    font-size: 13px;
  }
  .panel-title {
    font-size: 11px;
    --tw-font-weight: var(--font-weight-semibold);
    font-weight: var(--font-weight-semibold);
    color: var(--color-subtext1);
    text-transform: uppercase;
    letter-spacing: var(--title-letter-spacing);
    text-shadow: var(--title-text-shadow);
  }
  .panel-badge {
    margin-left: auto;
    min-width: calc(var(--spacing) * 5);
    border-radius: var(--radius-lg);
    background-color: var(--color-surface0);
    padding-inline: 7px;
    padding-block: 1px;
    text-align: center;
    font-size: 10px;
    --tw-font-weight: var(--font-weight-semibold);
    font-weight: var(--font-weight-semibold);
    color: var(--color-subtext0);
    border: var(--badge-border);
  }
  .panel-body {
    flex: 1;
    overflow: auto;
    padding: 10px 12px;
  }
  .panel-body::-webkit-scrollbar {
    width: 4px;
    height: 4px;
  }
  .panel-body::-webkit-scrollbar-thumb {
    background: var(--scrollbar-thumb);
    border-radius: 4px;
  }
  .panel-body::-webkit-scrollbar-track {
    background: var(--scrollbar-track);
  }
  .stat-mini {
    display: flex;
    align-items: center;
    gap: calc(var(--spacing) * 2.5);
    padding: 12px 14px;
  }
  .stat-mini .stat-icon-wrap {
    display: flex;
    height: calc(var(--spacing) * 9);
    width: calc(var(--spacing) * 9);
    flex-shrink: 0;
    align-items: center;
    justify-content: center;
    border-radius: 10px;
    color: var(--color-base);
  }
  .stat-mini .stat-icon-wrap.blue {
    background-color: var(--color-icon-bg-blue);
  }
  .stat-mini .stat-icon-wrap.green {
    background-color: var(--color-icon-bg-green);
  }
  .stat-mini .stat-icon-wrap.mauve {
    background-color: var(--color-icon-bg-mauve);
  }
  .stat-mini .stat-icon-wrap.peach {
    background-color: var(--color-icon-bg-peach);
  }
  .stat-mini .stat-icon-wrap.teal {
    background-color: var(--color-icon-bg-teal);
  }
  .stat-mini .stat-icon-wrap.pink {
    background-color: var(--color-icon-bg-pink);
  }
  .stat-info {
    display: flex;
    flex-direction: column;
  }
  .stat-value {
    font-size: 22px;
    --tw-leading: var(--leading-tight);
    line-height: var(--leading-tight);
    --tw-font-weight: var(--font-weight-bold);
    font-weight: var(--font-weight-bold);
    text-shadow: var(--stat-value-shadow);
  }
  .stat-label {
    font-size: 10px;
    --tw-font-weight: var(--font-weight-medium);
    font-weight: var(--font-weight-medium);
    text-transform: uppercase;
    color: var(--stat-label-color);
    letter-spacing: var(--stat-label-spacing);
  }
  table {
    width: 100%;
    border-collapse: collapse;
    font-family: var(--font-mono);
    font-size: var(--text-xs);
    line-height: var(--tw-leading, var(--text-xs--line-height));
  }
  thead {
    position: sticky;
    top: calc(var(--spacing) * 0);
    z-index: 1;
  }
  th {
    background-color: var(--color-base);
    padding: 6px 8px;
    text-align: left;
    font-size: 10px;
    --tw-font-weight: var(--font-weight-semibold);
    font-weight: var(--font-weight-semibold);
    color: var(--color-overlay1);
    text-transform: uppercase;
    letter-spacing: .4px;
    border-bottom: 1px solid var(--surface0);
    text-shadow: var(--th-text-shadow);
  }
  td {
    max-width: 260px;
    overflow: hidden;
    padding: 5px 8px;
    text-overflow: ellipsis;
    white-space: nowrap;
    color: var(--color-subtext1);
    border-bottom: 1px solid var(--td-border);
  }
  tr:hover td {
    background-color: var(--color-hover-row);
  }
  .table-wrap {
    max-height: 400px;
    overflow: auto;
  }
  a.xref {
    color: var(--color-sapphire);
    text-decoration-line: none;
    border-bottom: 1px dotted #74c7ec4d;
    transition: all var(--transition);
  }
  a.xref:hover {
    color: var(--color-blue);
    border-bottom-color: var(--blue);
  }
  .badge {
    display: inline-block;
    border-radius: var(--radius-md);
    padding-inline: 7px;
    padding-block: 1px;
    font-size: 10px;
    --tw-font-weight: var(--font-weight-semibold);
    font-weight: var(--font-weight-semibold);
    white-space: nowrap;
    border: var(--badge-border);
    text-shadow: var(--badge-text-shadow);
  }
  .badge-green {
    background-color: var(--color-icon-bg-green);
    color: var(--color-green);
  }
  .badge-yellow {
    color: var(--yellow);
    background: #f9e2af1f;
  }
  .badge-red {
    color: var(--red);
    background: #f38ba81f;
  }
  .badge-blue {
    background-color: var(--color-icon-bg-blue);
    color: var(--color-blue);
  }
  .badge-mauve {
    background-color: var(--color-icon-bg-mauve);
    color: var(--color-mauve);
  }
  .badge-peach {
    background-color: var(--color-icon-bg-peach);
    color: var(--color-peach);
  }
  .badge-teal {
    background-color: var(--color-icon-bg-teal);
    color: var(--color-teal);
  }
  .mono-addr {
    font-family: var(--font-mono);
    font-size: 11px;
    color: var(--color-sapphire);
  }
  .kv-row {
    display: flex;
    align-items: baseline;
    gap: calc(var(--spacing) * 2);
    padding-block: 1px;
    font-size: 11px;
  }
  .kv-label {
    min-width: 70px;
    flex-shrink: 0;
    font-size: 10px;
    --tw-font-weight: var(--font-weight-medium);
    font-weight: var(--font-weight-medium);
    color: var(--color-overlay1);
    text-transform: uppercase;
    letter-spacing: .3px;
  }
  .kv-value {
    overflow: hidden;
    font-family: var(--font-mono);
    font-size: 11px;
    text-overflow: ellipsis;
    white-space: nowrap;
    color: var(--color-subtext1);
  }
  .progress-bar {
    height: calc(var(--spacing) * 1.5);
    flex: 1;
    overflow: hidden;
    border-radius: var(--radius-sm);
    background-color: var(--color-surface0);
  }
  .progress-fill {
    height: 100%;
    border-radius: var(--radius-sm);
    transition: width .3s;
  }
  .progress-fill.blue {
    background-color: var(--color-blue);
  }
  .progress-fill.green {
    background-color: var(--color-green);
  }
  .empty {
    padding-block: calc(var(--spacing) * 3);
    text-align: center;
    font-size: var(--text-xs);
    line-height: var(--tw-leading, var(--text-xs--line-height));
    color: var(--color-overlay0);
    font-style: italic;
  }
  .hidden {
    display: none !important;
  }
  .panel-about {
    margin-inline: calc(var(--spacing) * 4);
    margin-bottom: calc(var(--spacing) * 3);
  }
  .panel-about .panel-body {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: calc(var(--spacing) * 4);
    padding-block: calc(var(--spacing) * 1.5);
    font-size: 11px;
    color: var(--color-overlay0);
  }
  .panel-about a {
    color: var(--color-blue);
    text-decoration-line: none;
  }
  .panel-about a:hover {
    text-decoration-line: underline;
  }
}
:root {
  --crust: #11111b;
  --mantle: #181825;
  --base: #1e1e2e;
  --surface0: #313244;
  --surface1: #45475a;
  --surface2: #585b70;
  --overlay0: #6c7086;
  --overlay1: #7f849c;
  --text: #cdd6f4;
  --subtext0: #a6adc8;
  --subtext1: #bac2de;
  --blue: #89b4fa;
  --sapphire: #74c7ec;
  --lavender: #b4befe;
  --green: #a6e3a1;
  --yellow: #f9e2af;
  --peach: #fab387;
  --red: #f38ba8;
  --mauve: #cba6f7;
  --pink: #f5c2e7;
  --teal: #94e2d5;
  --flamingo: #f2cdcd;
  --ui-radius: 12px;
  --ui-radius-sm: 8px;
  --gap: 8px;
  --transition: 0.2s cubic-bezier(0.4, 0, 0.2, 1);
  --panel-bg: #1e1e2eb3;
  --panel-border: #31324499;
  --glow-blue: #89b4fa14;
  --topbar-bg: #11111bd9;
  --panel-header-bg: #18182580;
  --hover-row: #89b4fa0a;
  --hover-summary: #89b4fa0f;
  --td-border: #3132444d;
  --shadow-hover: 0 4px 24px #0003;
  --icon-bg-blue: #89b4fa1f;
  --icon-bg-green: #a6e3a11f;
  --icon-bg-mauve: #cba6f71f;
  --icon-bg-peach: #fab3871f;
  --icon-bg-teal: #94e2d51f;
  --icon-bg-pink: #f5c2e71f;
  --version-bg: #a6e3a11f;
  --nav-active-bg: #89b4fa1f;
  --search-icon-fill: %236c7086;
  --panel-glow: none;
  --panel-hover-glow: none;
  --panel-header-gradient: none;
  --title-text-shadow: none;
  --title-letter-spacing: 0.5px;
  --badge-border: none;
  --badge-text-shadow: none;
  --topbar-shadow: none;
  --brand-text-shadow: none;
  --brand-logo-shadow: none;
  --stat-value-shadow: none;
  --stat-label-color: var(--overlay1);
  --stat-label-spacing: 0.3px;
  --th-text-shadow: none;
  --scene-layer-bg: #89b4fa0f;
  --scene-layer-hover-bg: #89b4fa33;
  --scene-layer-hover-shadow: 0 0 8px #89b4fa66;
  --scrollbar-thumb: var(--surface1);
  --scrollbar-track: transparent;
  --body-after-content: none;
  --body-after-bg: none;
  --version-border: none;
  --version-text-shadow: none;
}
[data-theme="light"] {
  --crust: #dce0e8;
  --mantle: #e6e9ef;
  --base: #eff1f5;
  --surface0: #ccd0da;
  --surface1: #bcc0cc;
  --surface2: #acb0be;
  --overlay0: #9ca0b0;
  --overlay1: #8c8fa1;
  --text: #4c4f69;
  --subtext0: #6c6f85;
  --subtext1: #5c5f77;
  --blue: #1e66f5;
  --sapphire: #209fb5;
  --lavender: #7287fd;
  --green: #40a02b;
  --yellow: #df8e1d;
  --peach: #fe640b;
  --red: #d20f39;
  --mauve: #8839ef;
  --pink: #ea76cb;
  --teal: #179299;
  --flamingo: #dd7878;
  --panel-bg: #eff1f5d9;
  --panel-border: #ccd0dab3;
  --glow-blue: #1e66f50f;
  --topbar-bg: #e6e9efe6;
  --panel-header-bg: #dce0e899;
  --hover-row: #1e66f50a;
  --hover-summary: #1e66f50f;
  --td-border: #ccd0da80;
  --shadow-hover: 0 4px 24px #0000000f;
  --icon-bg-blue: #1e66f51a;
  --icon-bg-green: #40a02b1a;
  --icon-bg-mauve: #8839ef1a;
  --icon-bg-peach: #fe640b1a;
  --icon-bg-teal: #1792991a;
  --icon-bg-pink: #ea76cb1a;
  --version-bg: #40a02b1a;
  --nav-active-bg: #1e66f51a;
  --search-icon-fill: %239ca0b0;
  --scene-layer-bg: #1e66f50f;
  --scene-layer-hover-bg: #1e66f533;
  --scene-layer-hover-shadow: 0 0 8px #1e66f566;
}
[data-theme="cyber"] {
  --crust: #05080a;
  --mantle: #0a0e14;
  --base: #0d1117;
  --surface0: #1a1f2e;
  --surface1: #252b3b;
  --surface2: #2f3649;
  --overlay0: #4a5568;
  --overlay1: #718096;
  --text: #e0f0ff;
  --subtext0: #8ba4c0;
  --subtext1: #a0c4e8;
  --blue: #00e5ff;
  --sapphire: #00bcd4;
  --lavender: #b388ff;
  --green: #00ff9f;
  --yellow: #ffea00;
  --peach: #ff6e40;
  --red: #ff1744;
  --mauve: #e040fb;
  --pink: #ff4081;
  --teal: #1de9b6;
  --flamingo: #ff80ab;
  --ui-radius: 4px;
  --ui-radius-sm: 2px;
  --panel-bg: #0d1117d9;
  --panel-border: #00e5ff26;
  --glow-blue: #00e5ff0f;
  --topbar-bg: #05080aeb;
  --panel-header-bg: #0a0e14b3;
  --hover-row: #00e5ff0f;
  --hover-summary: #00e5ff14;
  --td-border: #00e5ff14;
  --shadow-hover: 0 0 20px #00e5ff1f, 0 0 40px #00e5ff0a;
  --icon-bg-blue: #00e5ff1a;
  --icon-bg-green: #00ff9f1a;
  --icon-bg-mauve: #e040fb1a;
  --icon-bg-peach: #ff6e401a;
  --icon-bg-teal: #1de9b61a;
  --icon-bg-pink: #ff40811a;
  --version-bg: #00ff9f1a;
  --nav-active-bg: #00e5ff1f;
  --search-icon-fill: %234a5568;
  --panel-glow: inset 0 0 30px #00e5ff05;
  --panel-hover-glow: inset 0 0 30px #00e5ff08;
  --panel-header-gradient: linear-gradient(90deg, #00e5ff0d 0%, transparent 100%);
  --title-text-shadow: 0 0 6px #00e5ff4d;
  --title-letter-spacing: 1.5px;
  --badge-border: 1px solid currentColor;
  --badge-text-shadow: 0 0 4px currentColor;
  --topbar-shadow: 0 1px 12px #00e5ff14;
  --brand-text-shadow: 0 0 8px #00e5ff66;
  --brand-logo-shadow: 0 0 10px #00e5ff80;
  --stat-value-shadow: 0 0 8px #00e5ff4d;
  --stat-label-color: #00e5ff80;
  --stat-label-spacing: 1px;
  --th-text-shadow: 0 0 4px #00e5ff33;
  --scene-layer-bg: #00e5ff0a;
  --scene-layer-hover-bg: #00e5ff26;
  --scene-layer-hover-shadow: 0 0 12px #00e5ff66;
  --scrollbar-thumb: #00e5ff33;
  --scrollbar-track: #00e5ff05;
  --body-after-content: "";
  --body-after-bg: repeating-linear-gradient(0deg, transparent, transparent 2px, #00e5ff04 2px, #00e5ff04 4px);
  --version-border: 1px solid #00ff9f4d;
  --version-text-shadow: 0 0 4px #00ff9f4d;
}
ui-dashboard .main {
  margin-inline: auto;
  max-width: 1800px;
  padding-inline: calc(var(--spacing) * 4);
  padding-block: calc(var(--spacing) * 3);
  padding-bottom: calc(var(--spacing) * 6);
}
ui-dashboard .bento {
  display: grid;
  grid-template-columns: repeat(12, minmax(0, 1fr));
  gap: var(--gap);
  grid-auto-rows: minmax(40px, auto);
}
ui-dashboard .panel-stat {
  grid-column: span 2;
}
ui-dashboard .panel-disp-trees {
  grid-column: span 12;
}
ui-dashboard .panel-disp-trees > .panel-body {
  overflow: auto;
}
ui-dashboard .panel-img-cache, ui-dashboard .panel-hdr-cache, ui-dashboard .panel-decoders, ui-dashboard .panel-animations, ui-dashboard .panel-timers, ui-dashboard .panel-indevs, ui-dashboard .panel-groups, ui-dashboard .panel-draw-units, ui-dashboard .panel-draw-tasks, ui-dashboard .panel-subjects, ui-dashboard .panel-fs-drivers {
  grid-column: span 4;
}
ui-dashboard .panel-img-cache > .panel-body, ui-dashboard .panel-hdr-cache > .panel-body {
  max-height: 400px;
  overflow-y: auto;
}
ui-dashboard .panel-hdr-cache .table-wrap {
  overflow-x: auto;
}
ui-dashboard .panel-hdr-cache table {
  font-size: 11px;
}
ui-dashboard .panel-animations, ui-dashboard .panel-timers, ui-dashboard .panel-indevs, ui-dashboard .panel-groups, ui-dashboard .panel-draw-units, ui-dashboard .panel-draw-tasks, ui-dashboard .panel-subjects, ui-dashboard .panel-fs-drivers, ui-dashboard .panel-decoders {
  max-height: 360px;
}
ui-dashboard .panel-animations > .panel-body, ui-dashboard .panel-timers > .panel-body, ui-dashboard .panel-indevs > .panel-body, ui-dashboard .panel-groups > .panel-body, ui-dashboard .panel-draw-units > .panel-body, ui-dashboard .panel-draw-tasks > .panel-body, ui-dashboard .panel-subjects > .panel-body, ui-dashboard .panel-fs-drivers > .panel-body, ui-dashboard .panel-decoders > .panel-body {
  overflow-y: auto;
}
@media (max-width: 1200px) {
  ui-dashboard .bento {
    grid-template-columns: repeat(6, 1fr);
  }
  ui-dashboard .panel-stat {
    grid-column: span 2;
  }
  ui-dashboard .panel-disp-trees {
    grid-column: span 6;
  }
  ui-dashboard .panel-disp-trees .obj-split {
    height: auto;
    flex-direction: column;
  }
  ui-dashboard .panel-disp-trees .obj-split > .obj-tree-view, ui-dashboard .panel-disp-trees .obj-split > .obj-detail-view {
    max-height: 300px;
    flex: none;
  }
  ui-dashboard .panel-img-cache, ui-dashboard .panel-hdr-cache, ui-dashboard .panel-decoders {
    grid-column: span 6;
  }
  ui-dashboard .panel-animations, ui-dashboard .panel-timers, ui-dashboard .panel-indevs, ui-dashboard .panel-groups, ui-dashboard .panel-draw-units, ui-dashboard .panel-draw-tasks, ui-dashboard .panel-subjects, ui-dashboard .panel-fs-drivers {
    grid-column: span 3;
  }
}
@media (max-width: 768px) {
  ui-dashboard .bento {
    grid-template-columns: 1fr;
  }
  ui-dashboard .bento > * {
    grid-column: span 1;
  }
}
ui-drop-zone #drop-zone {
  margin-inline: calc(var(--spacing) * 4);
  margin-block: calc(var(--spacing) * 3);
  display: none;
  flex: 1;
  cursor: pointer;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  border-radius: var(--radius-xl);
  padding: calc(var(--spacing) * 12);
  text-align: center;
  color: var(--color-overlay1);
  border: 2px dashed var(--surface1);
  transition: all var(--transition);
}
ui-drop-zone #drop-zone:hover, ui-drop-zone #drop-zone.dragover {
  border-color: var(--color-blue);
  background-color: var(--color-glow-blue);
}
ui-drop-zone .drop-icon {
  margin-bottom: calc(var(--spacing) * 2);
  font-size: var(--text-4xl);
  line-height: var(--tw-leading, var(--text-4xl--line-height));
}
ui-drop-zone .drop-hint {
  margin-top: calc(var(--spacing) * 1.5);
  font-size: 11px;
  color: var(--color-overlay0);
}
ui-drop-zone .drop-hint code {
  border-radius: 0.25rem;
  background-color: var(--color-surface0);
  padding-inline: calc(var(--spacing) * 1.5);
  padding-block: calc(var(--spacing) * 0.5);
}
ui-drop-zone #file-input {
  display: none;
}
ui-theme-toggle .theme-toggle {
  display: flex;
  height: calc(var(--spacing) * 8);
  width: calc(var(--spacing) * 8);
  flex-shrink: 0;
  cursor: pointer;
  align-items: center;
  justify-content: center;
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  font-size: 15px;
  color: var(--color-overlay1);
  border: 1px solid var(--surface0);
  transition: all var(--transition);
}
ui-theme-toggle .theme-toggle:hover {
  border-color: var(--color-blue);
  color: var(--color-blue);
}
ui-topbar .topbar {
  position: sticky;
  top: calc(var(--spacing) * 0);
  z-index: 50;
  display: flex;
  height: calc(var(--spacing) * 11);
  align-items: center;
  gap: calc(var(--spacing) * 4);
  padding-inline: calc(var(--spacing) * 5);
  background: var(--topbar-bg);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid var(--panel-border);
  box-shadow: var(--topbar-shadow);
}
ui-topbar .topbar-brand {
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  font-size: 13px;
  --tw-font-weight: var(--font-weight-bold);
  font-weight: var(--font-weight-bold);
  white-space: nowrap;
  color: var(--color-blue);
  text-shadow: var(--brand-text-shadow);
}
ui-topbar .topbar-brand .logo {
  display: flex;
  height: 22px;
  width: 22px;
  align-items: center;
  justify-content: center;
  border-radius: var(--radius-md);
  background-color: var(--color-blue);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-extrabold);
  font-weight: var(--font-weight-extrabold);
  color: var(--color-crust);
  box-shadow: var(--brand-logo-shadow);
}
ui-topbar .topbar-sep {
  height: calc(var(--spacing) * 5);
  width: 1px;
  background-color: var(--color-surface0);
}
ui-topbar .topbar-meta {
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2.5);
  font-size: 11px;
  color: var(--color-overlay1);
}
ui-topbar .version-tag {
  border-radius: 5px;
  background-color: var(--color-version-bg);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 0.5);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-green);
  border: var(--version-border);
  text-shadow: var(--version-text-shadow);
}
ui-topbar .topbar-nav {
  margin-left: auto;
  display: flex;
  gap: calc(var(--spacing) * 0.5);
}
ui-topbar .topbar-nav a {
  border-radius: var(--radius-md);
  padding-inline: calc(var(--spacing) * 2.5);
  padding-block: calc(var(--spacing) * 1);
  font-size: 11px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  white-space: nowrap;
  color: var(--color-subtext0);
  text-decoration-line: none;
  transition: all var(--transition);
}
ui-topbar .topbar-nav a:hover {
  background-color: var(--color-surface0);
  color: var(--color-txt);
}
ui-topbar .topbar-nav a.active {
  background-color: var(--color-nav-active-bg);
  color: var(--color-blue);
}
ui-topbar .topbar-search {
  margin-left: calc(var(--spacing) * 2);
  width: calc(var(--spacing) * 40);
  border-radius: var(--radius-md);
  background-color: var(--color-base);
  padding-block: calc(var(--spacing) * 1);
  padding-right: calc(var(--spacing) * 2.5);
  padding-left: calc(var(--spacing) * 7);
  font-size: 11px;
  color: var(--color-txt);
  --tw-outline-style: none;
  outline-style: none;
  border: 1px solid var(--surface0);
  transition: all var(--transition);
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' fill='var(--search-icon-fill)' viewBox='0 0 24 24'%3E%3Cpath d='M15.5 14h-.79l-.28-.27A6.47 6.47 0 0 0 16 9.5 6.5 6.5 0 1 0 9.5 16c1.61 0 3.09-.59 4.23-1.57l.27.28v.79l5 4.99L20.49 19l-4.99-5zm-6 0C7.01 14 5 11.99 5 9.5S7.01 5 9.5 5 14 7.01 14 9.5 11.99 14 9.5 14z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: 8px center;
}
ui-topbar .topbar-search:focus {
  width: 220px;
  border-color: var(--color-blue);
}
ui-topbar .topbar-search::placeholder {
  color: var(--color-overlay0);
}
@media (max-width: 768px) {
  ui-topbar .topbar-nav {
    display: none;
  }
}
.disp-tab-bar {
  margin-bottom: calc(var(--spacing) * 2);
  display: flex;
  gap: calc(var(--spacing) * 1);
  padding-bottom: calc(var(--spacing) * 2);
  border-bottom: 1px solid var(--surface0);
}
.disp-tab-btn {
  display: flex;
  cursor: pointer;
  align-items: center;
  gap: calc(var(--spacing) * 1.5);
  background-color: var(--color-mantle);
  padding-inline: calc(var(--spacing) * 3);
  padding-block: calc(var(--spacing) * 1);
  font-family: var(--font-mono);
  font-size: 11px;
  color: var(--color-subtext0);
  border: 1px solid var(--surface0);
  border-bottom: none;
  border-radius: var(--ui-radius-sm) var(--ui-radius-sm) 0 0;
  transition: background var(--transition), color var(--transition);
}
.disp-tab-btn:hover {
  background-color: var(--color-surface0);
}
.disp-tab-btn.active {
  background-color: var(--color-base);
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-blue);
  border-color: var(--blue);
  border-bottom: 2px solid var(--base);
}
.disp-tab-thumb {
  height: calc(var(--spacing) * 12);
  width: calc(var(--spacing) * 12);
  border-radius: 0.25rem;
  background-color: var(--color-crust);
  object-fit: contain;
  border: 1px solid var(--surface0);
  image-rendering: pixelated;
}
.disp-info-bar {
  margin-bottom: calc(var(--spacing) * 1);
  display: flex;
  flex-wrap: wrap;
  gap: calc(var(--spacing) * 1.5);
  padding-bottom: calc(var(--spacing) * 2);
  border-bottom: 1px solid var(--surface0);
}
.disp-chip {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: calc(var(--spacing) * 1.5);
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  padding-inline: calc(var(--spacing) * 2.5);
  padding-block: 5px;
  font-size: 11px;
  border: 1px solid var(--surface0);
}
.disp-addr {
  font-family: var(--font-mono);
  font-size: 11px;
  color: var(--color-sapphire);
}
.disp-res {
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-txt);
}
.disp-screens {
  font-size: 11px;
  color: var(--color-overlay1);
}
.obj-tree-header {
  margin-bottom: calc(var(--spacing) * 1);
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 1);
  font-size: .75rem;
  color: var(--color-overlay1);
  border-bottom: 1px solid var(--surface0);
}
.obj-tree-expand-btn {
  margin-left: auto;
  cursor: pointer;
  border-radius: 0.25rem;
  background-color: var(--color-base);
  padding-inline: calc(var(--spacing) * 1.5);
  padding-block: calc(var(--spacing) * 0.5);
  font-size: 10px;
  color: var(--color-overlay0);
  border: 1px solid var(--surface0);
  transition: all var(--transition);
}
.obj-tree-expand-btn:hover {
  border-color: var(--color-blue);
  color: var(--color-blue);
}
.obj-split {
  display: flex;
  height: 700px;
  align-items: stretch;
  gap: calc(var(--spacing) * 2);
}
.obj-split > .obj-tree-view {
  min-width: calc(var(--spacing) * 0);
  flex: 0 0 220px;
  overflow: auto;
  font-size: 11px;
}
.obj-split > .obj-3d-view {
  position: relative;
  display: flex;
  min-width: calc(var(--spacing) * 0);
  flex: 1;
  flex-direction: column;
}
.obj-split > .obj-detail-view {
  min-width: calc(var(--spacing) * 0);
  flex: 0 0 280px;
  overflow: auto;
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  padding: calc(var(--spacing) * 2.5);
  font-size: var(--text-xs);
  line-height: var(--tw-leading, var(--text-xs--line-height));
  border: 1px solid var(--surface0);
}
.obj-node {
  margin-left: calc(var(--spacing) * 3);
}
.obj-node > summary {
  cursor: pointer;
  list-style-type: none;
  border-radius: 0.25rem;
  padding-inline: 6px;
  padding-block: 3px;
  padding-left: calc(var(--spacing) * 1);
  font-family: var(--font-mono);
  font-size: 11px;
  color: var(--color-subtext1);
  transition: background var(--transition);
}
.obj-node > summary::-webkit-details-marker {
  display: none;
}
.obj-node > summary::before {
  content: "";
  display: inline-block;
  width: 6px;
  height: 6px;
  border-radius: 50%;
  vertical-align: middle;
  margin-right: 5px;
  flex-shrink: 0;
  background: var(--depth-color, var(--overlay0));
}
.obj-node[open] > summary::before {
  border-radius: 2px;
}
.obj-node > summary:hover {
  background-color: var(--color-hover-summary);
}
.obj-summary-hint {
  margin-left: calc(var(--spacing) * 1);
  color: var(--color-overlay0);
}
.obj-node.obj-selected > summary {
  background-color: var(--color-nav-active-bg);
  outline: 1px solid var(--blue);
}
.detail-header {
  margin-bottom: calc(var(--spacing) * 2.5);
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  padding-bottom: calc(var(--spacing) * 2);
  border-bottom: 1px solid var(--surface0);
}
.detail-class {
  font-size: var(--text-sm);
  line-height: var(--tw-leading, var(--text-sm--line-height));
  --tw-font-weight: var(--font-weight-bold);
  font-weight: var(--font-weight-bold);
  color: var(--color-blue);
}
.detail-summary-text {
  border-radius: 0.25rem;
  background-color: var(--color-mantle);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 1.5);
  font-family: var(--font-mono);
  font-size: 12px;
  color: var(--color-overlay1);
  border: 1px solid var(--surface0);
}
.detail-section {
  margin-bottom: calc(var(--spacing) * 2.5);
}
.detail-section-title {
  margin-bottom: calc(var(--spacing) * 1);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-bold);
  font-weight: var(--font-weight-bold);
  color: var(--color-overlay0);
  text-transform: uppercase;
  letter-spacing: .5px;
}
.detail-coord-grid {
  display: flex;
  gap: calc(var(--spacing) * 3);
  font-family: var(--font-mono);
  font-size: 11px;
}
.detail-coord-group {
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 1.5);
  border-radius: 0.25rem;
  background-color: var(--color-mantle);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 1);
  border: 1px solid var(--surface0);
}
.detail-coord-label {
  font-size: 9px;
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-overlay1);
  text-transform: uppercase;
}
.detail-coord-val {
  color: var(--color-txt);
}
.detail-style-card {
  margin-bottom: calc(var(--spacing) * 1);
  border-radius: var(--radius-lg);
  background-color: var(--color-mantle);
  padding: 6px 8px;
  border: 1px solid var(--surface0);
}
.detail-style-hdr {
  margin-bottom: calc(var(--spacing) * 1);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-mauve);
}
.detail-style-table {
  width: 100%;
  font-size: 11px;
}
.detail-style-table th {
  padding: 1px 4px;
  text-align: left;
  font-size: 9px;
}
.detail-style-table td {
  padding: 1px 4px;
}
.detail-flags-wrap {
  display: flex;
  flex-wrap: wrap;
  gap: calc(var(--spacing) * 1);
}
.detail-flag-badge {
  display: inline-block;
  border-radius: 0.25rem;
  background-color: var(--color-surface0);
  padding-inline: calc(var(--spacing) * 1.5);
  padding-block: 1px;
  font-family: var(--font-mono);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  color: var(--color-overlay1);
  border: 1px solid var(--surface0);
}
.detail-state-active {
  border-color: var(--color-yellow);
  @supports (color: color-mix(in lab, red, red)) {
    border-color: color-mix(in oklab, var(--color-yellow) 30%, transparent);
  }
  background-color: var(--color-yellow);
  @supports (color: color-mix(in lab, red, red)) {
    background-color: color-mix(in oklab, var(--color-yellow) 10%, transparent);
  }
  color: var(--color-yellow);
}
.detail-adv-toggle {
  margin-top: calc(var(--spacing) * 1);
}
.detail-adv-summary {
  cursor: pointer;
  padding-block: calc(var(--spacing) * 0.5);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  color: var(--color-overlay0);
  list-style: none;
}
.detail-adv-summary::before {
  content: "▸ ";
}
.detail-adv-toggle[open] > .detail-adv-summary::before {
  content: "▾ ";
}
.scene-controls {
  margin-bottom: calc(var(--spacing) * 1);
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 1.5);
}
.scene-label {
  font-size: 10px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  color: var(--color-overlay1);
  text-transform: uppercase;
  letter-spacing: .3px;
}
.scene-slider {
  height: calc(var(--spacing) * 1);
  width: calc(var(--spacing) * 30);
  cursor: pointer;
  accent-color: var(--blue);
}
.scene-reset-btn, .scene-toggle-btn, .scene-fullscreen-btn {
  display: flex;
  cursor: pointer;
  align-items: center;
  gap: calc(var(--spacing) * 1);
  border-radius: 0.25rem;
  background-color: var(--color-base);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 0.5);
  font-size: 10px;
  color: var(--color-overlay1);
  border: 1px solid var(--surface0);
  transition: all var(--transition);
}
.scene-reset-btn:hover, .scene-toggle-btn:hover, .scene-fullscreen-btn:hover {
  border-color: var(--color-blue);
  color: var(--color-blue);
}
.scene-spacer {
  flex: 1;
}
.obj-3d-view.screensaver .scene-controls, .obj-3d-view.screensaver .scene-layer-bar, .obj-3d-view.screensaver .scene-tooltip {
  display: none !important;
}
.obj-3d-view.screensaver .scene-viewport {
  min-height: calc(var(--spacing) * 0);
  border-radius: 0;
  border-style: var(--tw-border-style);
  border-width: 0px;
}
.scene-toggle-btn {
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
}
.scene-toggle-btn.active {
  border-color: var(--color-blue);
  background-color: var(--color-icon-bg-blue);
  color: var(--color-blue);
}
.scene-layer-bar {
  display: flex;
  flex-wrap: wrap;
  gap: calc(var(--spacing) * 1);
  padding-block: calc(var(--spacing) * 1);
}
.scene-layer-btn {
  cursor: pointer;
  border-radius: 0.25rem;
  background-color: var(--color-base);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 0.5);
  font-family: var(--font-mono);
  font-size: 10px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  color: var(--color-overlay0);
  -webkit-user-select: none;
  user-select: none;
  border: 1px solid var(--surface0);
  border-bottom: 2px solid var(--surface1);
  transition: all var(--transition);
}
.scene-layer-btn:hover {
  border-color: var(--color-blue);
  color: var(--color-txt);
}
.scene-layer-btn.active {
  border-color: var(--color-blue);
  background-color: var(--color-icon-bg-blue);
  color: var(--color-txt);
}
.scene-viewport {
  position: relative;
  min-height: 400px;
  width: 100%;
  flex: 1;
  cursor: grab;
  overflow: hidden;
  border-radius: var(--radius-lg);
  background-color: var(--color-crust);
  border: 1px solid var(--surface0);
}
.obj-3d-view:fullscreen {
  display: flex;
  flex-direction: column;
  background-color: var(--color-crust);
  padding: calc(var(--spacing) * 2);
}
.obj-3d-view:fullscreen .scene-viewport {
  min-height: calc(var(--spacing) * 0);
}
.scene-canvas {
  display: block;
  height: 100%;
  width: 100%;
}
.scene-tooltip {
  pointer-events: none;
  position: absolute;
  z-index: 50;
  display: none;
  border-radius: var(--radius-md);
  background-color: var(--color-base);
  padding-inline: calc(var(--spacing) * 2);
  padding-block: calc(var(--spacing) * 1);
  font-family: var(--font-mono);
  font-size: 10px;
  white-space: nowrap;
  color: var(--color-txt);
  border: 1px solid var(--surface0);
  box-shadow: 0 4px 12px #0000004d;
}
.viewcube-wrap {
  position: absolute;
  z-index: 40;
  perspective: 250px;
}
.obj-3d-view.screensaver .viewcube-wrap {
  display: none !important;
}
.viewcube {
  width: 100%;
  height: 100%;
  position: relative;
  transform-style: preserve-3d;
  transition: transform 0.01s;
}
.vc-face, .vc-edge, .vc-corner {
  position: absolute;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  font: 600 8px/1 system-ui;
  color: var(--overlay1);
  backface-visibility: visible;
  transition: background 0.15s, color 0.15s;
  user-select: none;
}
.vc-face {
  background: var(--surface0);
  border: 1px solid var(--surface1);
  opacity: 0.85;
}
.vc-face:hover {
  background: var(--blue);
  color: var(--base);
  opacity: 1;
}
.vc-edge {
  background: var(--surface1);
  opacity: 0.6;
}
.vc-edge:hover {
  background: var(--blue);
  opacity: 1;
}
.vc-corner {
  width: 15px;
  height: 15px;
  background: var(--surface1);
  opacity: 0.5;
  border-radius: 2px;
}
.vc-corner:hover {
  background: var(--blue);
  opacity: 1;
}
.subject-card {
  margin-bottom: calc(var(--spacing) * 1.5);
  overflow: hidden;
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  border: 1px solid var(--surface0);
  &:last-child {
    margin-bottom: calc(var(--spacing) * 0);
  }
}
.subject-header {
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  padding: 6px 10px;
  font-size: 11px;
  border-bottom: 1px solid var(--surface0);
}
.subject-addr {
  font-family: var(--font-mono);
  color: var(--color-sapphire);
}
.subject-type {
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-mauve);
}
.subject-card table {
  font-size: 11px;
}
.timer-card, .anim-card, .indev-card, .group-card, .dtask-card {
  margin-bottom: calc(var(--spacing) * 1.5);
  overflow: hidden;
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  border: 1px solid var(--surface0);
  &:last-child {
    margin-bottom: calc(var(--spacing) * 0);
  }
}
.timer-info-row {
  display: flex;
  flex-wrap: wrap;
  gap: 4px 16px;
}
.anim-header, .timer-header, .indev-header, .group-header, .dtask-header {
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
  padding: 6px 10px;
  border-bottom: 1px solid var(--surface0);
}
.anim-info, .timer-info, .indev-info, .group-info, .dtask-info {
  padding: 6px 10px;
}
.anim-value-row {
  margin-block: calc(var(--spacing) * 1);
  display: flex;
  align-items: center;
  gap: calc(var(--spacing) * 2);
}
.anim-val-label {
  font-family: var(--font-mono);
  font-size: 10px;
  white-space: nowrap;
  color: var(--color-overlay1);
}
.anim-cur-val {
  text-align: right;
  font-size: 10px;
  color: var(--color-overlay1);
}
.indev-type-icon {
  font-size: var(--text-lg);
  line-height: var(--tw-leading, var(--text-lg--line-height));
}
.indev-type-name {
  font-size: var(--text-xs);
  line-height: var(--tw-leading, var(--text-xs--line-height));
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-txt);
}
.group-members {
  margin-top: calc(var(--spacing) * 1);
}
.member-list {
  display: flex;
  flex-wrap: wrap;
  gap: calc(var(--spacing) * 1);
  font-family: var(--font-mono);
  font-size: 10px;
}
.cache-grid {
  display: grid;
  gap: calc(var(--spacing) * 1.5);
  grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
}
.cache-entry {
  display: flex;
  align-items: center;
  overflow: hidden;
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  border: 1px solid var(--surface0);
}
.cache-thumb {
  margin: calc(var(--spacing) * 1.5);
  height: calc(var(--spacing) * 9);
  width: calc(var(--spacing) * 9);
  flex-shrink: 0;
  border-radius: 0.25rem;
  background-color: var(--color-surface0);
  object-fit: contain;
  image-rendering: pixelated;
}
.cache-info {
  min-width: calc(var(--spacing) * 0);
  flex: 1;
  padding: 4px 8px;
}
.cache-src {
  margin-bottom: calc(var(--spacing) * 0.5);
  overflow: hidden;
  font-size: 10px;
  --tw-font-weight: var(--font-weight-medium);
  font-weight: var(--font-weight-medium);
  text-overflow: ellipsis;
  white-space: nowrap;
  color: var(--color-txt);
}
.cache-meta-row {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 3px;
}
.cache-size-label {
  font-size: 10px;
  color: var(--color-overlay1);
}
.cache-decoder-label {
  margin-top: calc(var(--spacing) * 0.5);
  font-size: 10px;
  color: var(--color-overlay0);
}
.unit-grid, .decoder-grid, .fs-grid {
  display: grid;
  gap: calc(var(--spacing) * 1.5);
  grid-template-columns: repeat(auto-fill, minmax(140px, 1fr));
}
.unit-card, .decoder-card, .fs-card {
  border-radius: var(--radius-lg);
  background-color: var(--color-base);
  padding: calc(var(--spacing) * 2.5);
  text-align: center;
  border: 1px solid var(--surface0);
}
.unit-name, .decoder-name {
  margin-bottom: calc(var(--spacing) * 0.5);
  font-size: var(--text-xs);
  line-height: var(--tw-leading, var(--text-xs--line-height));
  --tw-font-weight: var(--font-weight-semibold);
  font-weight: var(--font-weight-semibold);
  color: var(--color-txt);
}
.unit-idx {
  font-size: var(--text-lg);
  line-height: var(--tw-leading, var(--text-lg--line-height));
  --tw-font-weight: var(--font-weight-bold);
  font-weight: var(--font-weight-bold);
  color: var(--color-blue);
}
.decoder-cbs {
  margin-block: calc(var(--spacing) * 1.5);
  display: flex;
  justify-content: center;
  gap: calc(var(--spacing) * 1);
}
.fs-letter {
  font-family: var(--font-mono);
  font-size: var(--text-2xl);
  line-height: var(--tw-leading, var(--text-2xl--line-height));
  --tw-font-weight: var(--font-weight-extrabold);
  font-weight: var(--font-weight-extrabold);
  color: var(--color-blue);
}
.fs-driver-name {
  margin-bottom: calc(var(--spacing) * 1);
  font-size: 11px;
  color: var(--color-subtext0);
}
.fs-info {
  font-size: 11px;
}
.fs-cbs {
  margin-top: calc(var(--spacing) * 1);
  display: flex;
  justify-content: center;
  gap: 3px;
}
@property --tw-font-weight {
  syntax: "*";
  inherits: false;
}
@property --tw-leading {
  syntax: "*";
  inherits: false;
}
@property --tw-border-style {
  syntax: "*";
  inherits: false;
  initial-value: solid;
}
@layer properties {
  @supports ((-webkit-hyphens: none) and (not (margin-trim: inline))) or ((-moz-orient: inline) and (not (color:rgb(from red r g b)))) {
    *, ::before, ::after, ::backdrop {
      --tw-font-weight: initial;
      --tw-leading: initial;
      --tw-border-style: solid;
    }
  }
}
</style>
</head>
<body>
<ui-topbar></ui-topbar>
  <ui-drop-zone></ui-drop-zone>
  <ui-dashboard></ui-dashboard>
  <footer class="panel panel-about"><div class="panel-body" id="about"></div></footer>
  <script id="lvgl-data" type="application/json">{{JSON_DATA}}</script>
<script>// framework/base.ts
class BaseComponent extends HTMLElement {
  el;
  static __injected = /* @__PURE__ */ new Set;
  connectedCallback() {
    let ctor = this.constructor, tag = this.tagName.toLowerCase();
    if (ctor.__style && !BaseComponent.__injected.has(tag)) {
      BaseComponent.__injected.add(tag);
      let s = document.createElement("style");
      s.textContent = ctor.__style, document.head.appendChild(s);
    }
    if (ctor.__template)
      this.el = ctor.__template(), this.appendChild(this.el);
    this.render();
  }
  html(strings, ...values) {
    let t = document.createElement("template");
    return t.innerHTML = String.raw(strings, ...values), t.content.firstElementChild;
  }
  update() {
    this.innerHTML = "";
    let ctor = this.constructor;
    if (ctor.__template)
      this.el = ctor.__template(), this.appendChild(this.el);
    this.render();
  }
  $(selector) {
    return this.querySelector(selector);
  }
}

// src/components/ui-theme-toggle.ui.ts
class UiThemeToggle extends BaseComponent {
  static __style = "";
  static __template = () => {
    let _e = document.createElement("button");
    return _e.setAttribute("class", "theme-toggle"), _e.setAttribute("title", "Toggle theme"), _e.append("\\uD83C\\uDF19"), _e;
  };
  render() {
    let THEMES = ["dark", "light", "cyber"], ICONS = { dark: "\\uD83C\\uDF19", light: "☀️", cyber: "⚡" }, root = document.documentElement, apply = (theme) => {
      root.setAttribute("data-theme", theme), this.el.textContent = ICONS[theme] || "\\uD83C\\uDF19";
    }, saved = localStorage.getItem("lvgl-dash-theme");
    if (saved && THEMES.includes(saved))
      apply(saved);
    else if (matchMedia("(prefers-color-scheme: light)").matches)
      apply("light");
    this.el.onclick = () => {
      let cur = root.getAttribute("data-theme") || "dark", next = THEMES[(THEMES.indexOf(cur) + 1) % THEMES.length];
      apply(next), localStorage.setItem("lvgl-dash-theme", next);
    };
  }
}
customElements.define("ui-theme-toggle", UiThemeToggle);
// framework/signal.ts
function signal(initial) {
  let value = initial, listeners = /* @__PURE__ */ new Set;
  return {
    get val() {
      return value;
    },
    set val(v) {
      if (v === value)
        return;
      value = v, listeners.forEach((fn) => fn());
    },
    sub(fn) {
      return listeners.add(fn), fn(), () => listeners.delete(fn);
    }
  };
}
// src/state.ts
var hlAddr = signal(null), hlRegistry = {}, hlOverlays = {};
function registerHL(addr, el) {
  if (!addr)
    return;
  if (!hlRegistry[addr])
    hlRegistry[addr] = [];
  hlRegistry[addr].push(el);
}
function onHL(fn) {
  hlAddr.sub(() => fn(hlAddr.val));
}
var focusAddr = signal(null);
function onFocus(fn) {
  focusAddr.sub(() => {
    if (focusAddr.val)
      fn(focusAddr.val);
  });
}
function focusObj(addr) {
  focusAddr.val = null, focusAddr.val = addr;
}
function highlightObj(addr) {
  if (hlAddr.val === addr)
    return;
  if (clearHighlight(), hlAddr.val = addr, !addr)
    return;
  hlRegistry[addr]?.forEach((e) => e.classList.add("hl-active")), Object.values(hlOverlays).forEach((ov) => {
    let ctx = ov.canvas.getContext("2d");
    ctx.clearRect(0, 0, ov.canvas.width, ov.canvas.height);
    let obj = ov.objs.find((o) => o.addr === addr);
    if (obj) {
      let sx = ov.canvas.width / ov.w, sy = ov.canvas.height / ov.h, x = obj.x1 * sx, y = obj.y1 * sy, w = (obj.x2 - obj.x1) * sx, h = (obj.y2 - obj.y1) * sy;
      ctx.strokeStyle = "rgba(137, 180, 250, 0.9)", ctx.lineWidth = 2, ctx.setLineDash([4, 2]), ctx.strokeRect(x, y, w, h), ctx.fillStyle = "rgba(137, 180, 250, 0.15)", ctx.fillRect(x, y, w, h);
    }
  });
}
function clearHighlight() {
  if (!hlAddr.val)
    return;
  hlRegistry[hlAddr.val]?.forEach((e) => e.classList.remove("hl-active")), Object.values(hlOverlays).forEach((ov) => {
    ov.canvas.getContext("2d").clearRect(0, 0, ov.canvas.width, ov.canvas.height);
  }), hlAddr.val = null;
}
function resetHL() {
  clearHighlight();
  for (let k in hlRegistry)
    delete hlRegistry[k];
  for (let k in hlOverlays)
    delete hlOverlays[k];
}
var selectedAddr = signal(null), objDataMap = {};
function selectObj(addr) {
  let prev = selectedAddr.val;
  if (prev)
    document.getElementById("obj-" + prev)?.classList.remove("obj-selected");
  selectedAddr.val = addr, document.getElementById("obj-" + addr)?.classList.add("obj-selected");
}
var dashData = signal(null), widgetSpecs = {};
function setWidgetSpecs(specs) {
  widgetSpecs = specs;
}
function getWidgetSpec(className) {
  return widgetSpecs[className] || widgetSpecs["lv_" + className];
}
function widgetSummary(className, wd) {
  if (!wd)
    return "";
  let spec = getWidgetSpec(className);
  if (!spec?.summary_tpl)
    return "";
  return spec.summary_tpl.replace(/\\{(\\w+)\\}/g, (_, k) => {
    let v = wd[k];
    if (v == null)
      return "";
    let s = String(v);
    return s.length > 24 ? s.slice(0, 24) + "…" : s;
  });
}
function countObjects(trees) {
  let n = 0;
  function walk(obj) {
    n++, obj.children?.forEach(walk);
  }
  return trees.forEach((t) => t.screens.forEach(walk)), n;
}

// src/constants.ts
var XREF_TARGET = {
  parent_addr: "obj",
  group_addr: "group",
  display_addr: "disp",
  read_timer_addr: "timer",
  focused_addr: "obj",
  var_addr: "obj",
  user_data_addr: "obj",
  subject_addr: "subject",
  target_addr: "obj",
  decoded_addr: "imgcache"
}, SECTIONS = [
  { key: "displays", icon: "\\uD83D\\uDDA5", title: "Displays & Objects", cls: "panel-disp-trees" },
  { key: "animations", icon: "\\uD83C\\uDFAC", title: "Animations", cls: "panel-animations" },
  { key: "timers", icon: "⏱", title: "Timers", cls: "panel-timers" },
  { key: "image_cache", icon: "\\uD83D\\uDDBC", title: "Image Cache", cls: "panel-img-cache" },
  { key: "image_header_cache", icon: "\\uD83D\\uDCCB", title: "Header Cache", cls: "panel-hdr-cache" },
  { key: "indevs", icon: "\\uD83D\\uDD79", title: "Input Devices", cls: "panel-indevs" },
  { key: "groups", icon: "\\uD83D\\uDC65", title: "Groups", cls: "panel-groups" },
  { key: "draw_units", icon: "\\uD83C\\uDFA8", title: "Draw Units", cls: "panel-draw-units" },
  { key: "draw_tasks", icon: "\\uD83D\\uDCDD", title: "Draw Tasks", cls: "panel-draw-tasks" },
  { key: "subjects", icon: "\\uD83D\\uDCE1", title: "Subjects", cls: "panel-subjects" },
  { key: "image_decoders", icon: "\\uD83D\\uDD13", title: "Decoders", cls: "panel-decoders" },
  { key: "fs_drivers", icon: "\\uD83D\\uDCBE", title: "FS Drivers", cls: "panel-fs-drivers" }
], STAT_DEFS = [
  { label: "Displays", key: "displays", icon: "\\uD83D\\uDDA5", color: "blue", section: "disp-trees" },
  { label: "Objects", key: "_objects", icon: "\\uD83C\\uDF33", color: "green", section: "disp-trees" },
  { label: "Animations", key: "animations", icon: "\\uD83C\\uDFAC", color: "mauve", section: "animations" },
  { label: "Timers", key: "timers", icon: "⏱", color: "peach", section: "timers" },
  { label: "Img Cache", key: "image_cache", icon: "\\uD83D\\uDDBC", color: "teal", section: "img-cache" },
  { label: "Input Devs", key: "indevs", icon: "\\uD83D\\uDD79", color: "pink", section: "indevs" }
], DEPTH_COLORS = [
  "var(--blue)",
  "var(--green)",
  "var(--mauve)",
  "var(--peach)",
  "var(--teal)",
  "var(--pink)",
  "var(--yellow)",
  "var(--red)",
  "var(--sapphire)",
  "var(--lavender)",
  "var(--flamingo)"
], C = {
  INFINITE_REPEAT: 4294967295,
  VIEWPORT_SIZE: 520,
  PERSPECTIVE_DISTANCE: 1200,
  DEFAULT_ROT_X: -30,
  DEFAULT_ROT_Y: 30,
  MIN_ZOOM: 0.2,
  MAX_ZOOM: 10,
  ZOOM_SENSITIVITY: 0.01,
  ROTATION_SENSITIVITY: 0.4,
  SCREEN_GAP: 2,
  ANIM_DURATION: 500,
  CAM_DIR: -1,
  VC_SIZE: 100,
  VC_FACE: 50,
  VC_EDGE: 8,
  VC_CORNER: 15,
  VC_TOP: 8,
  VC_RIGHT: 8,
  KB_ACCEL: 0.4,
  KB_FRICTION: 0.85,
  KB_PAN_ACCEL: 0.8,
  KB_PAN_FRICTION: 0.92,
  KB_ZOOM_ACCEL: 0.005,
  KB_ZOOM_FRICTION: 0.85,
  KB_SPREAD_ACCEL: 0.5
}, INDEV_ICONS = {
  pointer: "\\uD83D\\uDC46",
  keypad: "⌨️",
  button: "\\uD83D\\uDD18",
  encoder: "\\uD83C\\uDF9B️",
  none: "❓"
};

// src/components/ui-topbar.ui.ts
class UiTopbar extends BaseComponent {
  static __style = "";
  static __template = () => {
    let _e = document.createElement("header");
    _e.setAttribute("class", "topbar");
    let _e1 = document.createElement("div");
    _e1.setAttribute("class", "topbar-brand");
    let _e2 = document.createElement("span");
    _e2.setAttribute("class", "logo"), _e2.append("LD"), _e1.append(_e2, "LVGL Dashboard");
    let _e3 = document.createElement("div");
    _e3.setAttribute("class", "topbar-sep");
    let _e4 = document.createElement("div");
    _e4.setAttribute("class", "topbar-meta"), _e4.setAttribute("id", "header-meta");
    let _e5 = document.createElement("nav");
    _e5.setAttribute("class", "topbar-nav"), _e5.setAttribute("id", "topbar-nav");
    let _e6 = document.createElement("ui-theme-toggle"), _e7 = document.createElement("input");
    return _e7.setAttribute("type", "text"), _e7.setAttribute("class", "topbar-search"), _e7.setAttribute("id", "search"), _e7.setAttribute("placeholder", "Filter..."), _e7.setAttribute("aria-label", "Search"), _e.append(_e1, _e3, _e4, _e5, _e6, _e7), _e;
  };
  render() {
    let meta = this.$("#header-meta"), nav = this.$("#topbar-nav"), search = this.$("#search");
    dashData.sub(() => {
      let data = dashData.val;
      if (!data)
        return;
      if (meta.innerHTML = "", meta.appendChild(document.createTextNode(data.meta?.timestamp || "")), data.meta?.lvgl_version) {
        let tag = document.createElement("span");
        tag.className = "version-tag", tag.textContent = "LVGL " + data.meta.lvgl_version, meta.appendChild(tag);
      }
      nav.innerHTML = "", SECTIONS.forEach((s) => {
        let count = data[s.key]?.length || 0;
        if (count === 0)
          return;
        let a = document.createElement("a");
        a.textContent = s.icon + " " + count;
        let targetId = "sec-" + s.cls.replace("panel-", "");
        a.href = "#" + targetId, a.title = s.title + " (" + count + ")", a.addEventListener("click", (e) => {
          e.preventDefault(), document.getElementById(targetId)?.scrollIntoView({ behavior: "smooth", block: "start" });
        }), nav.appendChild(a);
      });
    }), search.addEventListener("input", () => {
      let q = search.value.toLowerCase();
      document.querySelectorAll(".panel").forEach((p) => {
        if (p.classList.contains("panel-stat"))
          return;
        if (!q) {
          p.classList.remove("hidden");
          return;
        }
        p.classList.toggle("hidden", !p.textContent.toLowerCase().includes(q));
      });
    });
  }
}
customElements.define("ui-topbar", UiTopbar);

// src/components/ui-drop-zone.ui.ts
class UiDropZone extends BaseComponent {
  static __style = "";
  static __template = () => {
    let _e = document.createElement("div");
    _e.setAttribute("id", "drop-zone"), _e.setAttribute("style", "display:none");
    let _e1 = document.createElement("div");
    _e1.setAttribute("class", "drop-icon"), _e1.append("\\uD83D\\uDCC2");
    let _e2 = document.createElement("p");
    _e2.append("Drag &amp; drop a JSON file here, or click to select");
    let _e3 = document.createElement("div");
    _e3.setAttribute("class", "drop-hint");
    let _e4 = document.createElement("code");
    _e4.append("dump dashboard --json"), _e3.append("Exported via ", _e4);
    let _e5 = document.createElement("input");
    return _e5.setAttribute("type", "file"), _e5.setAttribute("id", "file-input"), _e5.setAttribute("accept", ".json"), _e5.setAttribute("aria-label", "Load JSON file"), _e.append(_e1, _e2, _e3, _e5), _e;
  };
  render() {
    let zone = this.el, input = this.$("#file-input"), raw = document.getElementById("lvgl-data")?.textContent?.trim();
    if (raw)
      try {
        dashData.val = JSON.parse(raw);
        return;
      } catch (e) {}
    zone.style.display = "flex", this.style.display = "flex", this.style.flexDirection = "column", this.style.flex = "1";
    let load = (file) => {
      let reader = new FileReader;
      reader.onload = () => {
        try {
          dashData.val = JSON.parse(reader.result), zone.style.display = "none", this.style.display = "none";
        } catch (e) {
          alert("Invalid JSON: " + e.message);
        }
      }, reader.readAsText(file);
    };
    zone.addEventListener("dragover", (e) => {
      e.preventDefault(), zone.classList.add("dragover");
    }), zone.addEventListener("dragleave", () => zone.classList.remove("dragover")), zone.addEventListener("drop", (e) => {
      if (e.preventDefault(), zone.classList.remove("dragover"), e.dataTransfer?.files[0])
        load(e.dataTransfer.files[0]);
    }), zone.addEventListener("click", () => input.click()), input.addEventListener("change", () => {
      if (input.files?.[0])
        load(input.files[0]);
    });
  }
}
customElements.define("ui-drop-zone", UiDropZone);

// src/helpers.ts
function el(tag, cls, text) {
  let e = document.createElement(tag);
  if (cls)
    e.className = cls;
  if (text !== void 0)
    e.textContent = text;
  return e;
}
function xref(addr, prefix) {
  if (!addr || addr === "None" || addr === "0x0")
    return document.createTextNode(addr || "-");
  let a = el("a", "xref", addr);
  return a.href = "#" + prefix + "-" + addr, a;
}
function xrefCell(td, key, val) {
  let prefix = XREF_TARGET[key];
  if (prefix && val && val !== "None" && val !== "0x0")
    td.appendChild(xref(val, prefix));
  else
    td.textContent = val != null ? String(val) : "-";
}
function badge(text, color) {
  return el("span", "badge badge-" + color, text);
}
function kvPair(label, value) {
  let row = el("div", "kv-row");
  return row.appendChild(el("span", "kv-label", label)), row.appendChild(el("span", "kv-value", String(value))), row;
}
function progressBar(ratio, color) {
  let wrap = el("div", "progress-bar"), fill = el("div", "progress-fill " + color);
  return fill.style.width = Math.max(0, Math.min(100, ratio * 100)) + "%", wrap.appendChild(fill), wrap;
}
function makeTable(headers, rows, anchorPrefix) {
  if (!rows || rows.length === 0)
    return el("p", "empty", "No entries.");
  let wrap = el("div", "table-wrap"), tbl = document.createElement("table"), hr = tbl.createTHead().insertRow();
  headers.forEach((h) => {
    let th = document.createElement("th");
    th.textContent = h, hr.appendChild(th);
  });
  let tbody = tbl.createTBody();
  return rows.forEach((row) => {
    let tr = tbody.insertRow(), addr = row.addr;
    if (anchorPrefix && addr)
      tr.id = anchorPrefix + "-" + addr;
    headers.forEach((h) => {
      let key = h.toLowerCase().replace(/ /g, "_"), td = tr.insertCell(), val = row[key];
      if (key === "area" && typeof val === "object") {
        let a = val;
        td.textContent = "(" + a.x1 + "," + a.y1 + "," + a.x2 + "," + a.y2 + ")";
      } else if ((key === "member_addrs" || key === "observer_addrs") && Array.isArray(val))
        val.forEach((a, i) => {
          if (i > 0)
            td.appendChild(document.createTextNode(", "));
          td.appendChild(xref(a, key === "member_addrs" ? "obj" : "observer"));
        });
      else
        xrefCell(td, key, val != null ? String(val) : "-");
    });
  }), wrap.appendChild(tbl), wrap;
}
function makeStatPanel(icon, label, value, color, section) {
  let panel = el("div", "panel panel-stat");
  if (section)
    panel.style.cursor = "pointer", panel.addEventListener("click", () => {
      document.getElementById("sec-" + section)?.scrollIntoView({ behavior: "smooth", block: "start" });
    });
  let body = el("div", "stat-mini");
  body.appendChild(el("div", "stat-icon-wrap " + color, icon));
  let info = el("div", "stat-info");
  return info.appendChild(el("div", "stat-value", String(value))), info.appendChild(el("div", "stat-label", label)), body.appendChild(info), panel.appendChild(body), panel;
}
function makePanel(cls, icon, title, count) {
  let panel = el("div", "panel " + cls);
  panel.id = "sec-" + cls.replace("panel-", "");
  let hdr = el("div", "panel-header");
  if (hdr.appendChild(el("span", "panel-icon", icon)), hdr.appendChild(el("span", "panel-title", title)), count !== void 0)
    hdr.appendChild(el("span", "panel-badge", String(count)));
  panel.appendChild(hdr);
  let body = el("div", "panel-body");
  return panel.appendChild(body), { panel, body };
}
function buildCard(item, config) {
  let card = el("div", config.cardClass);
  if (config.anchorPrefix && item.addr)
    card.id = config.anchorPrefix + "-" + item.addr;
  let hdr = el("div", config.cardClass.replace("-card", "-header"));
  hdr.appendChild(el("span", "mono-addr", item.addr || "")), config.badges?.forEach((b) => hdr.appendChild(badge(b.text, b.color))), card.appendChild(hdr);
  let info = el("div", config.cardClass.replace("-card", "-info"));
  return config.content?.(info, item), card.appendChild(info), card;
}

// src/builders/obj-tree.ui.ts
function formatField(v, spec) {
  if (v == null)
    return "-";
  if (spec) {
    if (spec.type === "enum" && spec.names && typeof v === "number")
      return spec.names[v] ?? String(v);
    if (spec.type === "bool")
      return v ? "true" : "false";
    if (spec.type === "string" && typeof v === "string")
      return v.length > 60 ? '"' + v.slice(0, 60) + '…"' : '"' + v + '"';
  }
  if (typeof v === "object")
    return JSON.stringify(v);
  return String(v);
}
function renderObjTree(obj, depth = 0) {
  let det = document.createElement("details");
  if (det.className = "obj-node", obj.addr)
    det.id = "obj-" + obj.addr;
  let sum = document.createElement("summary");
  sum.style.setProperty("--depth-color", DEPTH_COLORS[depth % DEPTH_COLORS.length]);
  let nameText = (obj.name ? obj.name + " " : "") + (obj.class_name || "obj");
  sum.textContent = nameText;
  let hint = widgetSummary(obj.class_name, obj.widget_data);
  if (hint) {
    let span = document.createElement("span");
    span.className = "obj-summary-hint", span.textContent = hint, sum.appendChild(span);
  }
  if (obj.flags_list?.includes("HIDDEN"))
    sum.textContent += " \\uD83D\\uDC41‍\\uD83D\\uDDE8";
  if (det.appendChild(sum), obj.addr)
    objDataMap[obj.addr] = obj, registerHL(obj.addr, det), sum.addEventListener("mouseenter", () => highlightObj(obj.addr)), sum.addEventListener("mouseleave", () => clearHighlight()), sum.addEventListener("click", (e) => {
      e.stopPropagation(), selectObj(obj.addr);
    }), sum.addEventListener("dblclick", (e) => {
      e.stopPropagation(), focusObj(obj.addr);
    });
  return obj.children?.forEach((ch) => det.appendChild(renderObjTree(ch, depth + 1))), det;
}
function renderObjDetail(addr, panel) {
  panel.innerHTML = "";
  let obj = objDataMap[addr];
  if (!obj) {
    panel.appendChild(el("p", "empty", "Select an object to inspect."));
    return;
  }
  let summary = widgetSummary(obj.class_name, obj.widget_data), hdr = (() => {
    let _e = document.createElement("div");
    _e.setAttribute("class", "detail-header");
    let _e1 = document.createElement("span");
    _e1.setAttribute("class", "detail-class"), _e1.append(obj.class_name || "obj");
    let _e2 = document.createElement("span");
    return _e2.setAttribute("class", "mono-addr"), _e2.append(obj.addr), _e.append(_e1, _e2), _e;
  })();
  if (panel.appendChild(hdr), summary) {
    let sumSec = el("div", "detail-section");
    sumSec.appendChild(el("div", "detail-summary-text", summary)), panel.appendChild(sumSec);
  }
  let c = obj.coords || { x1: 0, y1: 0, x2: 0, y2: 0 }, w = (c.x2 || 0) - (c.x1 || 0), h = (c.y2 || 0) - (c.y1 || 0), coordSec = (() => {
    let _e = document.createElement("div");
    _e.setAttribute("class", "detail-section");
    let _e1 = document.createElement("div");
    _e1.setAttribute("class", "detail-section-title"), _e1.append("Coordinates");
    let _e2 = document.createElement("div");
    _e2.setAttribute("class", "detail-coord-grid");
    let _e3 = document.createElement("div");
    _e3.setAttribute("class", "detail-coord-group");
    let _e4 = document.createElement("span");
    _e4.setAttribute("class", "detail-coord-label"), _e4.append("pos");
    let _e5 = document.createElement("span");
    _e5.setAttribute("class", "detail-coord-val"), _e5.append(String(c.x1 || 0), ", ", String(c.y1 || 0));
    let _e6 = document.createElement("span");
    _e6.setAttribute("class", "detail-coord-label"), _e6.append("→");
    let _e7 = document.createElement("span");
    _e7.setAttribute("class", "detail-coord-val"), _e7.append(String(c.x2 || 0), ", ", String(c.y2 || 0)), _e3.append(_e4, _e5, _e6, _e7);
    let _e8 = document.createElement("div");
    _e8.setAttribute("class", "detail-coord-group");
    let _e9 = document.createElement("span");
    _e9.setAttribute("class", "detail-coord-label"), _e9.append("size");
    let _e10 = document.createElement("span");
    return _e10.setAttribute("class", "detail-coord-val"), _e10.append(String(w), " × ", String(h)), _e8.append(_e9, _e10), _e2.append(_e3, _e8), _e.append(_e1, _e2), _e;
  })();
  if (panel.appendChild(coordSec), obj.flags_list?.length) {
    let flagSec = el("div", "detail-section");
    flagSec.appendChild(el("div", "detail-section-title", "Flags"));
    let wrap = el("div", "detail-flags-wrap");
    obj.flags_list.forEach((f) => wrap.appendChild(el("span", "detail-flag-badge", f))), flagSec.appendChild(wrap), panel.appendChild(flagSec);
  }
  if (obj.state_list?.length) {
    let stateSec = el("div", "detail-section");
    stateSec.appendChild(el("div", "detail-section-title", "State"));
    let wrap = el("div", "detail-flags-wrap");
    obj.state_list.forEach((s) => wrap.appendChild(el("span", "detail-flag-badge" + (s === "DEFAULT" ? "" : " detail-state-active"), s))), stateSec.appendChild(wrap), panel.appendChild(stateSec);
  }
  let refSec = el("div", "detail-section");
  if (refSec.appendChild(el("div", "detail-section-title", "References")), obj.parent_addr) {
    let row = el("div", "kv-row");
    row.appendChild(el("span", "kv-label", "parent")), row.appendChild(xref(obj.parent_addr, "obj")), refSec.appendChild(row);
  }
  if (obj.group_addr) {
    let row = el("div", "kv-row");
    row.appendChild(el("span", "kv-label", "group")), row.appendChild(xref(obj.group_addr, "group")), refSec.appendChild(row);
  }
  if (obj.user_data)
    refSec.appendChild(kvPair("user_data", obj.user_data));
  if (obj.name)
    refSec.appendChild(kvPair("name", obj.name));
  refSec.appendChild(kvPair("children", String(obj.child_count || 0))), refSec.appendChild(kvPair("styles", String(obj.style_count || 0))), panel.appendChild(refSec);
  let _n = (v) => v != null;
  if (obj.scroll || _n(obj.ext_click_pad) || _n(obj.ext_draw_size) || _n(obj.scrollbar_mode) || _n(obj.layer_type) || obj.w_layout || obj.h_layout) {
    let layoutSec = el("div", "detail-section");
    if (layoutSec.appendChild(el("div", "detail-section-title", "Layout & Scroll")), obj.scroll)
      layoutSec.appendChild(kvPair("scroll", `${obj.scroll.x}, ${obj.scroll.y}`));
    if (_n(obj.ext_click_pad))
      layoutSec.appendChild(kvPair("ext_click_pad", String(obj.ext_click_pad)));
    if (_n(obj.ext_draw_size))
      layoutSec.appendChild(kvPair("ext_draw_size", String(obj.ext_draw_size)));
    if (_n(obj.scrollbar_mode))
      layoutSec.appendChild(kvPair("scrollbar_mode", String(obj.scrollbar_mode)));
    if (_n(obj.scroll_dir))
      layoutSec.appendChild(kvPair("scroll_dir", String(obj.scroll_dir)));
    if (_n(obj.scroll_snap_x))
      layoutSec.appendChild(kvPair("scroll_snap_x", String(obj.scroll_snap_x)));
    if (_n(obj.scroll_snap_y))
      layoutSec.appendChild(kvPair("scroll_snap_y", String(obj.scroll_snap_y)));
    if (_n(obj.layer_type))
      layoutSec.appendChild(kvPair("layer_type", String(obj.layer_type)));
    if (obj.w_layout)
      layoutSec.appendChild(kvPair("w_layout", "true"));
    if (obj.h_layout)
      layoutSec.appendChild(kvPair("h_layout", "true"));
    panel.appendChild(layoutSec);
  }
  if (obj.layout_inv || obj.is_deleting || obj.rendered || obj.skip_trans) {
    let intSec = el("div", "detail-section");
    intSec.appendChild(el("div", "detail-section-title", "Internal"));
    let wrap = el("div", "detail-flags-wrap");
    if (obj.layout_inv)
      wrap.appendChild(el("span", "detail-flag-badge detail-state-active", "layout_inv"));
    if (obj.is_deleting)
      wrap.appendChild(el("span", "detail-flag-badge detail-state-active", "is_deleting"));
    if (obj.rendered)
      wrap.appendChild(el("span", "detail-flag-badge", "rendered"));
    if (obj.skip_trans)
      wrap.appendChild(el("span", "detail-flag-badge", "skip_trans"));
    intSec.appendChild(wrap), panel.appendChild(intSec);
  }
  if (obj.widget_data && Object.keys(obj.widget_data).length) {
    let wd = obj.widget_data, spec = getWidgetSpec(obj.class_name), fieldSpecs = spec?.fields || {}, primary = spec?.primary || [], allKeys = Object.keys(wd), priKeys = primary.filter((k) => (k in wd)), advKeys = allKeys.filter((k) => !priKeys.includes(k)), wdSec = el("div", "detail-section");
    wdSec.appendChild(el("div", "detail-section-title", "Widget · " + obj.class_name));
    let renderField = (k) => {
      let fs = fieldSpecs[k], raw = wd[k];
      wdSec.appendChild(kvPair(k, formatField(raw, fs)));
    };
    for (let k of priKeys.length ? priKeys : allKeys.slice(0, 6))
      renderField(k);
    let rest = priKeys.length ? advKeys : allKeys.slice(6);
    if (rest.length) {
      let toggle = el("details", "detail-adv-toggle");
      toggle.appendChild(el("summary", "detail-adv-summary", `${rest.length} more fields`));
      let inner = el("div", "");
      for (let k of rest) {
        let fs = fieldSpecs[k];
        inner.appendChild(kvPair(k, formatField(wd[k], fs)));
      }
      toggle.appendChild(inner), wdSec.appendChild(toggle);
    }
    panel.appendChild(wdSec);
  }
  if (obj.styles?.length) {
    let styleSec = el("div", "detail-section");
    styleSec.appendChild(el("div", "detail-section-title", "Styles (" + obj.styles.length + ")")), obj.styles.forEach((s) => {
      let card = el("div", "detail-style-card");
      if (card.appendChild(el("div", "detail-style-hdr", "[" + s.index + "] " + s.selector_str + "  " + s.flags_str)), s.properties?.length) {
        let tbl = document.createElement("table");
        tbl.className = "detail-style-table";
        let thead = tbl.createTHead().insertRow();
        ["prop", "value"].forEach((h2) => {
          let th = document.createElement("th");
          th.textContent = h2, thead.appendChild(th);
        });
        let tbody = tbl.createTBody();
        s.properties.forEach((p) => {
          let r = tbody.insertRow();
          r.insertCell().textContent = p.prop_name, r.insertCell().textContent = p.value_str;
        }), card.appendChild(tbl);
      }
      styleSec.appendChild(card);
    }), panel.appendChild(styleSec);
  }
}

// src/builders/canvas2d-renderer.ts
class Canvas2DRenderer {
  canvas;
  ctx;
  layers = [];
  bufs = [];
  cam;
  sceneW = 0;
  sceneH = 0;
  hlAddr = null;
  dirty = !0;
  rafId = null;
  showLabels = !1;
  projected = [];
  constructor(canvas, cam) {
    this.canvas = canvas, this.ctx = canvas.getContext("2d"), this.cam = cam, this.startLoop();
  }
  setSceneSize(w, h) {
    this.sceneW = w, this.sceneH = h, this.dirty = !0;
  }
  setLayers(layers) {
    this.layers = layers, this.dirty = !0;
  }
  addBuf(buf) {
    this.bufs.push(buf), this.dirty = !0;
  }
  setHighlight(addr) {
    this.hlAddr = addr, this.dirty = !0;
  }
  markDirty() {
    this.dirty = !0;
  }
  project(px, py, pz) {
    let { rotX, rotY, zoom, panX, panY, persp } = this.cam, rx = -rotX * Math.PI / 180, ry = -rotY * Math.PI / 180, cx = Math.cos(rx), sx = Math.sin(rx), cy = Math.cos(ry), sy = Math.sin(ry), ox = px - this.sceneW / 2, oy = py - this.sceneH / 2, oz = -pz, x = ox * cy + oz * sy, z = -ox * sy + oz * cy, y = oy * cx - z * sx;
    z = oy * sx + z * cx;
    let p = C.PERSPECTIVE_DISTANCE;
    if (persp > 0 && z < -p * 0.95)
      return null;
    let rect = this.canvas.getBoundingClientRect(), cw = rect.width, ch = rect.height, perspScale = p / (p + z) * zoom, orthoScale = zoom, scale = orthoScale + (perspScale - orthoScale) * persp;
    return [
      cw / 2 + (x + panX) * scale,
      ch / 2 + (y + panY) * scale,
      z
    ];
  }
  pointInQuad(px, py, q) {
    for (let i = 0;i < 4; i++) {
      let [ax, ay] = q[i], [bx, by] = q[(i + 1) % 4];
      if ((bx - ax) * (py - ay) - (by - ay) * (px - ax) < 0)
        return !1;
    }
    return !0;
  }
  pick(mx, my) {
    for (let i = this.projected.length - 1;i >= 0; i--) {
      let p = this.projected[i];
      if (!p.layer.visible)
        continue;
      if (this.pointInQuad(mx, my, p.quad))
        return { layer: p.layer, index: i };
    }
    return null;
  }
  render() {
    let ctx = this.ctx, cw = this.canvas.width, ch = this.canvas.height;
    ctx.clearRect(0, 0, cw, ch);
    let items = [];
    this.bufs.forEach((buf) => {
      if (!buf.visible)
        return;
      let d = buf.depth, pts = [this.project(0, 0, d), this.project(this.sceneW, 0, d), this.project(this.sceneW, this.sceneH, d), this.project(0, this.sceneH, d)];
      if (pts.some((p) => !p))
        return;
      let [p0, p1, p2, p3] = pts, avgZ = (p0[2] + p1[2] + p2[2] + p3[2]) / 4;
      items.push({ quad: [[p0[0], p0[1]], [p1[0], p1[1]], [p2[0], p2[1]], [p3[0], p3[1]]], z: avgZ, type: "buf", buf });
    }), this.layers.forEach((l) => {
      if (!l.visible)
        return;
      let pts = [this.project(l.x, l.y, l.depth), this.project(l.x + l.w, l.y, l.depth), this.project(l.x + l.w, l.y + l.h, l.depth), this.project(l.x, l.y + l.h, l.depth)];
      if (pts.some((p) => !p))
        return;
      let [p0, p1, p2, p3] = pts, avgZ = (p0[2] + p1[2] + p2[2] + p3[2]) / 4;
      items.push({ quad: [[p0[0], p0[1]], [p1[0], p1[1]], [p2[0], p2[1]], [p3[0], p3[1]]], z: avgZ, type: "layer", layer: l });
    }), items.sort((a, b) => b.z - a.z), this.projected = items.filter((i) => i.type === "layer").map((i) => ({ quad: i.quad, layer: i.layer, z: i.z }));
    let SUBDIVS = 16;
    items.forEach((item) => {
      let q = item.quad;
      if (item.type === "buf" && item.buf) {
        let N = SUBDIVS, sw = this.sceneW, sh = this.sceneH, dpr = window.devicePixelRatio || 1, margin = 1 / N;
        for (let iy = 0;iy < N; iy++)
          for (let ix = 0;ix < N; ix++) {
            let u0 = ix / N, v0 = iy / N, u0e = Math.max(0, u0 - margin * 0.5), v0e = Math.max(0, v0 - margin * 0.5), u1e = Math.min(1, (ix + 1) / N + margin * 0.5), v1e = Math.min(1, (iy + 1) / N + margin * 0.5), p00 = this.project(sw * u0e, sh * v0e, item.buf.depth), p10 = this.project(sw * u1e, sh * v0e, item.buf.depth), p01 = this.project(sw * u0e, sh * v1e, item.buf.depth);
            if (!p00 || !p10 || !p01)
              continue;
            ctx.save(), ctx.globalAlpha = 0.85;
            let dx = p10[0] - p00[0], dy = p10[1] - p00[1], ex = p01[0] - p00[0], ey = p01[1] - p00[1], srcW = (u1e - u0e) * sw, srcH = (v1e - v0e) * sh;
            ctx.setTransform(dx / srcW * dpr, dy / srcW * dpr, ex / srcH * dpr, ey / srcH * dpr, p00[0] * dpr, p00[1] * dpr), ctx.drawImage(item.buf.img, u0e * sw, v0e * sh, srcW, srcH, 0, 0, srcW, srcH), ctx.restore();
          }
        if (this.hlAddr) {
          let hl = this.layers.find((l2) => l2.addr === this.hlAddr);
          if (hl) {
            let d = item.buf.depth, hp = [this.project(hl.x, hl.y, d), this.project(hl.x + hl.w, hl.y, d), this.project(hl.x + hl.w, hl.y + hl.h, d), this.project(hl.x, hl.y + hl.h, d)];
            if (hp.every(Boolean)) {
              let [h0, h1, h2, h3] = hp;
              ctx.beginPath(), ctx.moveTo(h0[0], h0[1]), ctx.lineTo(h1[0], h1[1]), ctx.lineTo(h2[0], h2[1]), ctx.lineTo(h3[0], h3[1]), ctx.closePath(), ctx.fillStyle = "rgba(137, 180, 250, 0.18)", ctx.fill(), ctx.setLineDash([4, 3]), ctx.strokeStyle = "rgba(137, 180, 250, 0.6)", ctx.lineWidth = 1.5, ctx.stroke(), ctx.setLineDash([]);
            }
          }
        }
        return;
      }
      let l = item.layer, isHl = l.addr === this.hlAddr;
      ctx.beginPath(), ctx.moveTo(q[0][0], q[0][1]);
      for (let i = 1;i < 4; i++)
        ctx.lineTo(q[i][0], q[i][1]);
      if (ctx.closePath(), ctx.fillStyle = isHl ? "rgba(137, 180, 250, 0.25)" : "rgba(137, 180, 250, 0.06)", ctx.fill(), ctx.strokeStyle = isHl ? "rgba(137, 180, 250, 0.9)" : l.borderColor, ctx.lineWidth = isHl ? 2 : 1, ctx.stroke(), this.showLabels && l.label) {
        let ax = (q[1][0] - q[0][0] + (q[2][0] - q[3][0])) / 2, ay = (q[1][1] - q[0][1] + (q[2][1] - q[3][1])) / 2, bx = (q[3][0] - q[0][0] + (q[2][0] - q[1][0])) / 2, by = (q[3][1] - q[0][1] + (q[2][1] - q[1][1])) / 2, qw = Math.hypot(ax, ay), qh = Math.hypot(bx, by);
        if (qw > 30) {
          let ox = (q[0][0] + q[1][0] + q[2][0] + q[3][0]) / 4, oy = (q[0][1] + q[1][1] + q[2][1] + q[3][1]) / 4;
          ctx.save(), ctx.transform(ax / qw, ay / qw, bx / qh, by / qh, ox, oy), ctx.font = "bold 10px system-ui", ctx.textAlign = "center", ctx.textBaseline = "middle", ctx.fillStyle = isHl ? "#89b4fa" : l.borderColor, ctx.fillText(l.label, 0, 0, qw * 0.9), ctx.restore();
        }
      }
    });
  }
  startLoop() {
    let tick = () => {
      if (this.dirty)
        this.dirty = !1, this.resizeCanvas(), this.render();
      this.rafId = requestAnimationFrame(tick);
    };
    this.rafId = requestAnimationFrame(tick);
  }
  resizeCanvas() {
    let rect = this.canvas.parentElement.getBoundingClientRect(), dpr = window.devicePixelRatio || 1, w = Math.round(rect.width * dpr), h = Math.round(rect.height * dpr);
    if (this.canvas.width !== w || this.canvas.height !== h)
      this.canvas.width = w, this.canvas.height = h, this.canvas.style.width = rect.width + "px", this.canvas.style.height = rect.height + "px", this.ctx.scale(dpr, dpr);
  }
  destroy() {
    if (this.rafId)
      cancelAnimationFrame(this.rafId);
  }
}

// src/builders/scene-3d.ui.ts
function flattenLayers(trees) {
  let layers = [], screenNames = [], screenMaxLocal = {}, globalOffset = 0, sIdx = 0;
  return trees.forEach((t) => (t.screens || []).forEach((s) => {
    let idx = sIdx++;
    screenNames.push(s.layer_name || s.class_name || "screen_" + idx);
    let maxLocal = 0;
    function walk(obj, ld, parentHidden) {
      let c = obj.coords || { x1: 0, y1: 0, x2: 0, y2: 0 }, hidden = parentHidden || (obj.flags_list?.includes("HIDDEN") ?? !1);
      layers.push({
        addr: obj.addr,
        class_name: obj.class_name || "obj",
        x1: c.x1 || 0,
        y1: c.y1 || 0,
        x2: c.x2 || 0,
        y2: c.y2 || 0,
        depth: globalOffset + ld,
        localDepth: ld,
        child_count: obj.child_count || 0,
        style_count: obj.style_count || 0,
        screenIdx: idx,
        hidden,
        summary: widgetSummary(obj.class_name, obj.widget_data)
      }), maxLocal = Math.max(maxLocal, ld), obj.children?.forEach((ch) => walk(ch, ld + 1, hidden));
    }
    walk(s, 0, !1), screenMaxLocal[idx] = maxLocal, globalOffset += maxLocal + C.SCREEN_GAP;
  })), { layers, screenNames, screenMaxLocal };
}
function resolveCSSColor(cssVar) {
  if (!cssVar.startsWith("var("))
    return cssVar;
  let name = cssVar.slice(4, -1);
  return getComputedStyle(document.documentElement).getPropertyValue(name).trim() || "#89b4fa";
}
function build3DScene(container, trees, displays, dispObjs) {
  let { layers, screenNames, screenMaxLocal } = flattenLayers(trees);
  if (!layers.length) {
    container.appendChild(el("p", "empty", "No objects."));
    return;
  }
  let alive = !0, maxX = 0, maxY = 0, maxDepth = 0;
  displays.forEach((d) => {
    maxX = Math.max(maxX, d.hor_res || 0), maxY = Math.max(maxY, d.ver_res || 0);
  }), layers.forEach((l) => {
    maxDepth = Math.max(maxDepth, l.depth);
  });
  let sceneW = maxX || 1, sceneH = maxY || 1, bufImages = [];
  displays.forEach((d) => {
    [d.buf_1, d.buf_2].forEach((b, i) => {
      if (!b?.image_base64)
        return;
      let img = new Image;
      img.src = "data:image/png;base64," + b.image_base64, bufImages.push({ img, label: "Buf" + (i + 1), base64: b.image_base64 });
    });
  });
  let cam = {
    rotX: C.DEFAULT_ROT_X,
    rotY: C.DEFAULT_ROT_Y,
    zoom: 1,
    panX: 0,
    panY: 0,
    persp: 1
  }, makeToggle = (label, on) => {
    let btn = el("button", "scene-toggle-btn" + (on ? " active" : ""), label);
    return btn.dataset.on = on ? "1" : "0", btn.onclick = () => {
      let v = btn.dataset.on === "1";
      btn.dataset.on = v ? "0" : "1", btn.classList.toggle("active", !v);
    }, btn;
  }, controls = el("div", "scene-controls"), toggle3d = makeToggle("3D", !0), toggleBorders = makeToggle("Borders", !0), bufToggles = [], toggleOrtho = makeToggle("Ortho", !1);
  controls.append(toggle3d, toggleBorders);
  let toggleHidden = makeToggle("Hidden", !1), toggleLabels = makeToggle("Props", !1);
  controls.appendChild(toggleHidden), controls.appendChild(toggleLabels), bufImages.forEach((bi) => {
    let btn = makeToggle(bi.label, !0), thumb = (() => {
      let _e = document.createElement("img");
      return _e.setAttribute("draggable", "false"), _e.setAttribute("style", "height:1.2em;border-radius:2px;vertical-align:middle;image-rendering:pixelated"), _e;
    })();
    thumb.src = "data:image/png;base64," + bi.base64, btn.appendChild(thumb), bufToggles.push(btn), controls.appendChild(btn);
  }), controls.appendChild(toggleOrtho);
  let defaultSpread = maxDepth > 0 ? Math.round(300 / maxDepth) : 30, spreadMax = Math.max(500, defaultSpread * 5), spreadSlider = (() => {
    let _e = document.createElement("input");
    return _e.setAttribute("type", "range"), _e.setAttribute("min", "0"), _e.setAttribute("max", `${spreadMax}`), _e.setAttribute("value", `${defaultSpread}`), _e.setAttribute("class", "scene-slider"), _e;
  })();
  controls.appendChild(el("label", "scene-label", "Z Spread")), controls.appendChild(spreadSlider);
  let depthRange = { min: 0, max: maxDepth }, depthMinSlider = (() => {
    let _e = document.createElement("input");
    return _e.setAttribute("type", "range"), _e.setAttribute("min", "0"), _e.setAttribute("max", `${maxDepth}`), _e.setAttribute("value", "0"), _e.setAttribute("class", "scene-slider"), _e;
  })(), depthMaxSlider = (() => {
    let _e = document.createElement("input");
    return _e.setAttribute("type", "range"), _e.setAttribute("min", "0"), _e.setAttribute("max", `${maxDepth}`), _e.setAttribute("value", `${maxDepth}`), _e.setAttribute("class", "scene-slider"), _e;
  })();
  controls.appendChild(el("label", "scene-label", "Depth")), controls.appendChild(depthMinSlider), controls.appendChild(depthMaxSlider);
  let resetBtn = el("button", "scene-reset-btn", "Reset"), spacer = el("span", "scene-spacer"), fsBtn = el("button", "scene-fullscreen-btn", "⛶");
  fsBtn.title = "Fullscreen", fsBtn.onclick = () => {
    if (document.fullscreenElement)
      document.exitFullscreen();
    else
      container.requestFullscreen();
  }, controls.append(resetBtn, spacer);
  let inScreensaver = !1;
  controls.appendChild(fsBtn), container.appendChild(controls);
  let layerVisible = [], layerBtns = [];
  if (screenNames.length > 0) {
    let bar = el("div", "scene-layer-bar");
    screenNames.forEach((name, i) => {
      let on = name === "act_scr" || screenNames.length === 1;
      layerVisible.push(on);
      let btn = el("button", "scene-layer-btn" + (on ? " active" : ""), name);
      btn.style.borderBottomColor = DEPTH_COLORS[i % DEPTH_COLORS.length], btn.onclick = () => {
        layerVisible[i] = !layerVisible[i], btn.classList.toggle("active", layerVisible[i]), updateDepthSliderRange(), updateVisibility();
      }, btn.ondblclick = (e) => {
        if (e.preventDefault(), layerVisible.every((v, j) => j === i ? v : !v))
          layerVisible.fill(!0);
        else
          layerVisible.fill(!1), layerVisible[i] = !0;
        layerBtns.forEach((b, j) => b.classList.toggle("active", layerVisible[j])), updateDepthSliderRange(), updateVisibility();
      }, layerBtns.push(btn), bar.appendChild(btn);
    }), container.appendChild(bar);
  }
  let tooltip = el("div", "scene-tooltip");
  container.appendChild(tooltip);
  let viewport = el("div", "scene-viewport"), canvas = (() => {
    let _e = document.createElement("canvas");
    return _e.setAttribute("class", "scene-canvas"), _e;
  })();
  viewport.appendChild(canvas);
  let S = C.VC_FACE / 2, vcItems = [
    ...[
      [`translateZ(${S}px)`, 0, 0, "Front"],
      [`rotateY(180deg) translateZ(${S}px)`, 0, 180, "Back"],
      [`rotateY(90deg) translateZ(${S}px)`, 0, -90, "Right"],
      [`rotateY(-90deg) translateZ(${S}px)`, 0, 90, "Left"],
      [`rotateX(90deg) translateZ(${S}px)`, -90, 0, "Top"],
      [`rotateX(-90deg) translateZ(${S}px)`, 90, 0, "Bot"]
    ].map(([t, rx, ry, l]) => ({ transform: t, preset: { rotX: rx, rotY: ry }, w: C.VC_FACE, h: C.VC_FACE, label: l, cls: "vc-face" })),
    ...[
      [`translateY(-${S}px) translateZ(${S}px)`, -45, 0, C.VC_FACE, C.VC_EDGE],
      [`translateY(${S}px) translateZ(${S}px)`, 45, 0, C.VC_FACE, C.VC_EDGE],
      [`translateX(-${S}px) translateZ(${S}px)`, 0, 45, C.VC_EDGE, C.VC_FACE],
      [`translateX(${S}px) translateZ(${S}px)`, 0, -45, C.VC_EDGE, C.VC_FACE],
      [`translateY(-${S}px) translateZ(-${S}px) rotateY(180deg)`, -45, 180, C.VC_FACE, C.VC_EDGE],
      [`translateY(${S}px) translateZ(-${S}px) rotateY(180deg)`, 45, 180, C.VC_FACE, C.VC_EDGE],
      [`translateX(-${S}px) translateZ(-${S}px) rotateY(180deg)`, 0, 135, C.VC_EDGE, C.VC_FACE],
      [`translateX(${S}px) translateZ(-${S}px) rotateY(180deg)`, 0, -135, C.VC_EDGE, C.VC_FACE],
      [`translateX(-${S}px) translateY(-${S}px) rotateY(90deg)`, -45, 90, C.VC_FACE, C.VC_EDGE],
      [`translateX(${S}px) translateY(-${S}px) rotateY(90deg)`, -45, -90, C.VC_FACE, C.VC_EDGE],
      [`translateX(-${S}px) translateY(${S}px) rotateY(90deg)`, 45, 90, C.VC_FACE, C.VC_EDGE],
      [`translateX(${S}px) translateY(${S}px) rotateY(90deg)`, 45, -90, C.VC_FACE, C.VC_EDGE]
    ].map(([t, rx, ry, w, h]) => ({ transform: t, preset: { rotX: rx, rotY: ry }, w, h, cls: "vc-edge" })),
    ...[[-1, -1, 1, 45], [1, -1, 1, -45], [-1, 1, 1, 45], [1, 1, 1, -45], [-1, -1, -1, 135], [1, -1, -1, -135], [-1, 1, -1, 135], [1, 1, -1, -135]].map(([sx, sy, sz, ry]) => ({ transform: `translate3d(${sx * S}px,${sy * S}px,${sz * S}px)`, preset: { rotX: sy * 45, rotY: ry }, w: C.VC_CORNER, h: C.VC_CORNER, cls: "vc-corner" }))
  ], vcWrap = el("div", "viewcube-wrap");
  vcWrap.style.cssText = `top:${C.VC_TOP}px;right:${C.VC_RIGHT}px;width:${C.VC_SIZE}px;height:${C.VC_SIZE}px`;
  let vcCube = el("div", "viewcube"), C2 = C.VC_SIZE / 2;
  for (let item of vcItems) {
    let d = el("div", item.cls, item.label);
    d.style.cssText = `transform:${item.transform};width:${item.w}px;height:${item.h}px;left:${(C.VC_SIZE - item.w) / 2}px;top:${(C.VC_SIZE - item.h) / 2}px`, d.onclick = () => animateTo({ rotX: item.preset.rotX, rotY: item.preset.rotY }), vcCube.appendChild(d);
  }
  vcWrap.appendChild(vcCube), viewport.appendChild(vcWrap);
  function updateViewCube() {
    vcCube.style.transform = `rotateX(${cam.rotX}deg) rotateY(${cam.rotY}deg)`;
    let p = 250 + (1 - cam.persp) * 2000;
    vcWrap.style.perspective = `${p}px`;
  }
  container.appendChild(viewport);
  let renderer;
  renderer = new Canvas2DRenderer(canvas, cam), renderer.setSceneSize(sceneW, sceneH);
  let _origMarkDirty = renderer.markDirty.bind(renderer);
  renderer.markDirty = () => {
    _origMarkDirty(), updateViewCube();
  }, updateViewCube();
  let sceneLayers = layers.map((l) => ({
    x: l.x1,
    y: l.y1,
    w: Math.max(2, l.x2 - l.x1),
    h: Math.max(2, l.y2 - l.y1),
    depth: 0,
    borderColor: resolveCSSColor(DEPTH_COLORS[l.depth % DEPTH_COLORS.length]),
    addr: l.addr,
    info: l.class_name + "@" + (l.addr || "?") + " [" + l.x1 + "," + l.y1 + "," + l.x2 + "," + l.y2 + "] children=" + l.child_count + " styles=" + l.style_count,
    label: l.summary ? l.class_name + " " + l.summary : l.class_name,
    screenIdx: l.screenIdx,
    visible: !0
  })), sceneBufs = bufImages.map((bi) => ({ img: bi.img, depth: 0, visible: !0 }));
  sceneBufs.forEach((b) => renderer.addBuf(b)), renderer.setLayers(sceneLayers), onHL((addr) => renderer.setHighlight(addr)), onFocus((addr) => focusLayer(addr));
  let currentSpread = 0;
  function computeScreenOffsets() {
    let o = {}, off = 0;
    for (let i = 0;i < screenNames.length; i++)
      if (layerVisible[i])
        o[i] = off, off += (screenMaxLocal[i] || 0) + C.SCREEN_GAP;
    return o;
  }
  function updateDepthSliderRange() {
    let so = computeScreenOffsets(), visMax = 0;
    layers.forEach((l) => {
      if (so[l.screenIdx] !== void 0)
        visMax = Math.max(visMax, so[l.screenIdx] + l.localDepth);
    });
    let prevMax = Number(depthMaxSlider.max);
    if (depthMinSlider.max = String(visMax), depthMaxSlider.max = String(visMax), depthRange.max >= prevMax)
      depthRange.max = visMax, depthMaxSlider.value = String(visMax);
    else if (depthRange.max > visMax)
      depthRange.max = visMax, depthMaxSlider.value = String(visMax);
    if (depthRange.min > visMax)
      depthRange.min = visMax, depthMinSlider.value = String(visMax);
  }
  function updateDepths(spreadOv, rangeOv) {
    let bordersOn = toggleBorders.dataset.on === "1", showHidden = toggleHidden.dataset.on === "1", range = rangeOv ?? depthRange, screenOffset = computeScreenOffsets();
    currentSpread = spreadOv ?? (is3d ? Number(spreadSlider.value) : 0.1);
    let compressedIdx = 0, compressedDepth = {}, seenDepths = /* @__PURE__ */ new Set;
    layers.forEach((l) => {
      let gd = screenOffset[l.screenIdx] !== void 0 ? screenOffset[l.screenIdx] + l.localDepth : -1;
      if (gd >= Math.round(range.min) && gd <= Math.round(range.max) && !seenDepths.has(gd))
        seenDepths.add(gd), compressedDepth[gd] = compressedIdx++;
    }), layers.forEach((l, idx) => {
      let sl = sceneLayers[idx], gd = screenOffset[l.screenIdx] !== void 0 ? screenOffset[l.screenIdx] + l.localDepth : -1, inRange = gd >= Math.round(range.min) && gd <= Math.round(range.max);
      sl.visible = bordersOn && layerVisible[l.screenIdx] && inRange && (!l.hidden || showHidden), sl.depth = (compressedDepth[gd] ?? 0) * currentSpread;
    }), sceneBufs.forEach((b) => {
      b.depth = -currentSpread * 0.5;
    }), renderer.markDirty();
  }
  function updateVisibility() {
    bufToggles.forEach((btn, i) => {
      sceneBufs[i].visible = btn.dataset.on === "1";
    }), updateDepths();
  }
  let is3d = !0, savedRotX = C.DEFAULT_ROT_X, savedRotY = C.DEFAULT_ROT_Y, animId = null;
  function ease(t) {
    return t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2;
  }
  function animateTo(target, done) {
    if (animId)
      cancelAnimationFrame(animId), animId = null;
    let t = { rotX: cam.rotX, rotY: cam.rotY, zoom: cam.zoom, panX: cam.panX, panY: cam.panY, spread: currentSpread, persp: cam.persp, depthMin: depthRange.min, depthMax: depthRange.max, ...target }, from = { rotX: cam.rotX, rotY: cam.rotY, zoom: cam.zoom, panX: cam.panX, panY: cam.panY, spread: currentSpread, persp: cam.persp, depthMin: depthRange.min, depthMax: depthRange.max }, t0 = performance.now(), lerp = (a, b, p) => a + (b - a) * p;
    function tick(now) {
      let p = ease(Math.min((now - t0) / C.ANIM_DURATION, 1));
      if (cam.rotX = lerp(from.rotX, t.rotX, p), cam.rotY = lerp(from.rotY, t.rotY, p), cam.zoom = lerp(from.zoom, t.zoom, p), cam.panX = lerp(from.panX, t.panX, p), cam.panY = lerp(from.panY, t.panY, p), cam.persp = lerp(from.persp, t.persp, p), updateDepths(lerp(from.spread, t.spread, p), { min: lerp(from.depthMin, t.depthMin, p), max: lerp(from.depthMax, t.depthMax, p) }), now - t0 < C.ANIM_DURATION)
        animId = requestAnimationFrame(tick);
      else
        animId = null, cam.rotX = t.rotX, cam.rotY = t.rotY, cam.zoom = t.zoom, cam.panX = t.panX, cam.panY = t.panY, cam.persp = t.persp, depthRange = { min: t.depthMin, max: t.depthMax }, depthMinSlider.value = String(Math.round(t.depthMin)), depthMaxSlider.value = String(Math.round(t.depthMax)), spreadSlider.value = String(t.spread), updateDepths(), done?.();
    }
    animId = requestAnimationFrame(tick);
  }
  let savedSpread = defaultSpread, savedDepthRange = { min: 0, max: maxDepth }, savedFocusCam = { rotX: 0, rotY: 0, panX: 0, panY: 0, persp: 1 }, focusedAddr = null;
  function exitFocus() {
    if (!focusedAddr)
      return;
    focusedAddr = null, animateTo({ ...savedFocusCam, depthMin: savedDepthRange.min, depthMax: savedDepthRange.max });
  }
  function focusLayer(addr) {
    let l = layers.find((la) => la.addr === addr);
    if (!l)
      return;
    if (focusedAddr === addr) {
      exitFocus();
      return;
    }
    if (!focusedAddr)
      savedDepthRange = { ...depthRange }, savedFocusCam = { rotX: cam.rotX, rotY: cam.rotY, panX: cam.panX, panY: cam.panY, persp: cam.persp };
    focusedAddr = addr;
    let cx = l.x1 + (l.x2 - l.x1) / 2 - sceneW / 2, cy = l.y1 + (l.y2 - l.y1) / 2 - sceneH / 2, gd = (computeScreenOffsets()[l.screenIdx] ?? 0) + l.localDepth;
    animateTo({ rotX: 0, rotY: 0, panX: -cx, panY: -cy, persp: 0, depthMin: gd, depthMax: gd });
  }
  let savedPersp = cam.persp;
  function animateToggle(entering) {
    if (!entering)
      savedRotX = cam.rotX, savedRotY = cam.rotY, savedSpread = Number(spreadSlider.value) || defaultSpread, savedPersp = cam.persp;
    animateTo({ rotX: entering ? savedRotX : 0, rotY: entering ? savedRotY : 0, spread: entering ? savedSpread : 0, persp: entering ? savedPersp : 0 }, () => {
      spreadSlider.value = String(savedSpread);
    });
  }
  updateDepthSliderRange(), updateDepths(), spreadSlider.oninput = () => updateDepths(), depthMinSlider.oninput = () => {
    if (depthRange.min = Number(depthMinSlider.value), depthRange.min > depthRange.max)
      depthRange.max = depthRange.min, depthMaxSlider.value = depthMinSlider.value;
    updateDepths();
  }, depthMaxSlider.oninput = () => {
    if (depthRange.max = Number(depthMaxSlider.value), depthRange.max < depthRange.min)
      depthRange.min = depthRange.max, depthMinSlider.value = depthMaxSlider.value;
    updateDepths();
  }, toggle3d.addEventListener("click", () => {
    is3d = toggle3d.dataset.on === "1", spreadSlider.disabled = !is3d, animateToggle(is3d);
  }), toggleOrtho.addEventListener("click", () => {
    let ortho = toggleOrtho.dataset.on === "1";
    animateTo({ persp: ortho ? 0 : 1 });
  }), toggleBorders.addEventListener("click", () => updateVisibility()), toggleHidden.addEventListener("click", () => updateVisibility()), toggleLabels.addEventListener("click", () => {
    renderer.showLabels = toggleLabels.dataset.on === "1", renderer.markDirty();
  }), bufToggles.forEach((btn) => btn.addEventListener("click", () => updateVisibility()));
  let dragging = !1, lastX = 0, lastY = 0;
  viewport.onmousedown = (e) => {
    if (inScreensaver)
      return;
    if (e.button === 1 || e.button === 0 && e.shiftKey)
      e.preventDefault(), dragging = "pan", lastX = e.clientX, lastY = e.clientY, viewport.style.cursor = "move";
    else if (e.button === 0 && is3d)
      dragging = "rotate", lastX = e.clientX, lastY = e.clientY, viewport.style.cursor = "grabbing";
  };
  let onMouseMove = (e) => {
    if (!dragging) {
      let rect = canvas.getBoundingClientRect(), mx = e.clientX - rect.left, my = e.clientY - rect.top;
      if (mx < 0 || my < 0 || mx > rect.width || my > rect.height)
        return;
      let hit = renderer.pick(mx, my);
      if (hit) {
        tooltip.textContent = hit.layer.info, tooltip.style.display = "block";
        let cr = container.getBoundingClientRect();
        if (tooltip.style.left = e.clientX - cr.left + 12 + "px", tooltip.style.top = e.clientY - cr.top - 8 + "px", renderer.setHighlight(hit.layer.addr), hit.layer.addr)
          highlightObj(hit.layer.addr);
      } else
        tooltip.style.display = "none", renderer.setHighlight(null), clearHighlight();
      return;
    }
    let dx = e.clientX - lastX, dy = e.clientY - lastY;
    if (lastX = e.clientX, lastY = e.clientY, dragging === "rotate")
      exitFocus(), cam.rotY += dx * C.ROTATION_SENSITIVITY, cam.rotX = Math.max(-90, Math.min(90, cam.rotX - dy * C.ROTATION_SENSITIVITY));
    else
      cam.panX += dx / cam.zoom, cam.panY += dy / cam.zoom;
    renderer.markDirty();
  }, onMouseUp = () => {
    dragging = !1, viewport.style.cursor = "grab";
  };
  window.addEventListener("mousemove", onMouseMove), window.addEventListener("mouseup", onMouseUp), viewport.addEventListener("wheel", (e) => {
    if (e.preventDefault(), inScreensaver)
      return;
    if (e.ctrlKey)
      cam.zoom = Math.max(C.MIN_ZOOM, Math.min(C.MAX_ZOOM, cam.zoom * (1 - e.deltaY * C.ZOOM_SENSITIVITY)));
    else
      cam.panX -= e.deltaX / cam.zoom, cam.panY -= e.deltaY / cam.zoom;
    renderer.markDirty();
  }, { passive: !1 }), container.tabIndex = 0;
  let keysDown = /* @__PURE__ */ new Set;
  container.addEventListener("keydown", (e) => {
    if (inScreensaver)
      return;
    if (e.key === "Shift") {
      keysDown.add("shift");
      return;
    }
    switch (e.key) {
      case "ArrowUp":
      case "ArrowDown":
      case "ArrowLeft":
      case "ArrowRight":
      case "w":
      case "W":
      case "s":
      case "S":
      case "a":
      case "A":
      case "d":
      case "D":
      case "=":
      case "+":
      case "-":
      case "_":
      case "q":
      case "Q":
      case "e":
      case "E":
        e.preventDefault(), keysDown.add(e.key.toLowerCase());
        return;
      case "r":
      case "R":
        resetBtn.click();
        return;
      case " ":
        e.preventDefault(), toggle3d.click();
        return;
      case "o":
      case "O":
        toggleOrtho.click();
        return;
      case "b":
      case "B":
        toggleBorders.click();
        return;
      default: {
        let n = parseInt(e.key);
        if (n >= 1 && n <= layerBtns.length)
          layerBtns[n - 1].click();
      }
    }
  }), container.addEventListener("keyup", (e) => keysDown.delete(e.key.toLowerCase())), container.addEventListener("blur", () => keysDown.clear());
  let vel = { rotX: 0, rotY: 0, panX: 0, panY: 0, zoom: 0, spread: 0 };
  function tickKeys() {
    let s = keysDown.has("shift") ? 3 : 1;
    if (keysDown.has("arrowup") || keysDown.has("arrowdown") || keysDown.has("arrowleft") || keysDown.has("arrowright"))
      exitFocus();
    let d = C.CAM_DIR;
    if (keysDown.has("arrowup"))
      vel.rotX -= C.KB_ACCEL * s * d;
    if (keysDown.has("arrowdown"))
      vel.rotX += C.KB_ACCEL * s * d;
    if (keysDown.has("arrowleft"))
      vel.rotY += C.KB_ACCEL * s * d;
    if (keysDown.has("arrowright"))
      vel.rotY -= C.KB_ACCEL * s * d;
    if (keysDown.has("w"))
      vel.panY += C.KB_PAN_ACCEL * s * d / cam.zoom;
    if (keysDown.has("s"))
      vel.panY -= C.KB_PAN_ACCEL * s * d / cam.zoom;
    if (keysDown.has("a"))
      vel.panX += C.KB_PAN_ACCEL * s * d / cam.zoom;
    if (keysDown.has("d"))
      vel.panX -= C.KB_PAN_ACCEL * s * d / cam.zoom;
    if (keysDown.has("=") || keysDown.has("+"))
      vel.zoom += C.KB_ZOOM_ACCEL * s;
    if (keysDown.has("-") || keysDown.has("_"))
      vel.zoom -= C.KB_ZOOM_ACCEL * s;
    if (keysDown.has("e"))
      vel.spread += C.KB_SPREAD_ACCEL * s;
    if (keysDown.has("q"))
      vel.spread -= C.KB_SPREAD_ACCEL * s;
    if (cam.rotX = Math.max(-90, Math.min(90, cam.rotX + vel.rotX)), cam.rotY += vel.rotY, cam.panX += vel.panX, cam.panY += vel.panY, cam.zoom = Math.max(C.MIN_ZOOM, Math.min(C.MAX_ZOOM, cam.zoom * (1 + vel.zoom))), Math.abs(vel.spread) > 0.01) {
      let nv = Math.max(0, Math.min(spreadMax, Number(spreadSlider.value) + vel.spread));
      spreadSlider.value = String(nv), updateDepths();
    }
    if (vel.rotX *= C.KB_FRICTION, vel.rotY *= C.KB_FRICTION, vel.panX *= C.KB_PAN_FRICTION, vel.panY *= C.KB_PAN_FRICTION, vel.zoom *= C.KB_ZOOM_FRICTION, vel.spread *= C.KB_FRICTION, Math.abs(vel.rotX) + Math.abs(vel.rotY) + Math.abs(vel.panX) + Math.abs(vel.panY) + Math.abs(vel.zoom) + Math.abs(vel.spread) > 0.001 || keysDown.size > 0)
      renderer.markDirty();
  }
  (function keyLoop() {
    if (!alive)
      return;
    tickKeys(), requestAnimationFrame(keyLoop);
  })();
  let pickAt = (e) => {
    let r = canvas.getBoundingClientRect();
    return renderer.pick(e.clientX - r.left, e.clientY - r.top);
  };
  canvas.addEventListener("click", (e) => {
    if (inScreensaver)
      return;
    let hit = pickAt(e);
    if (hit?.layer.addr) {
      selectObj(hit.layer.addr);
      let el2 = document.getElementById("obj-" + hit.layer.addr);
      if (el2) {
        let p = el2.parentElement;
        while (p) {
          if (p.tagName === "DETAILS")
            p.open = !0;
          p = p.parentElement;
        }
        el2.open = !0, el2.scrollIntoView({ behavior: "smooth", block: "nearest" });
      }
    }
  }), canvas.addEventListener("dblclick", (e) => {
    if (inScreensaver)
      return;
    let hit = pickAt(e);
    if (hit?.layer.addr)
      focusLayer(hit.layer.addr);
  });
  let setToggle = (btn, on) => {
    btn.dataset.on = on ? "1" : "0", btn.classList.toggle("active", on);
  };
  return resetBtn.onclick = () => {
    is3d = !0, setToggle(toggle3d, !0), setToggle(toggleBorders, !0), setToggle(toggleOrtho, !1), setToggle(toggleHidden, !1), setToggle(toggleLabels, !1), renderer.showLabels = !1, bufToggles.forEach((btn) => setToggle(btn, !0)), screenNames.forEach((name, i) => {
      layerVisible[i] = name === "act_scr" || screenNames.length === 1;
    }), layerBtns.forEach((b, i) => b.classList.toggle("active", layerVisible[i])), spreadSlider.disabled = !1, focusedAddr = null, updateDepthSliderRange(), updateVisibility();
    let visMax = Number(depthMaxSlider.max);
    animateTo({ rotX: C.DEFAULT_ROT_X, rotY: C.DEFAULT_ROT_Y, zoom: 1, panX: 0, panY: 0, spread: defaultSpread, persp: 1, depthMin: 0, depthMax: visMax });
  }, () => {
    if (alive = !1, animId)
      cancelAnimationFrame(animId);
    renderer.destroy(), window.removeEventListener("mousemove", onMouseMove), window.removeEventListener("mouseup", onMouseUp);
  };
}

// src/builders/display.ui.ts
function buildDisplayAndTrees(data) {
  let displays = data.displays || [], trees = data.object_trees || [], objCount = countObjects(trees), { panel, body } = makePanel("panel-disp-trees", "\\uD83D\\uDDA5", "Displays & Objects", displays.length + " disp / " + objCount + " obj");
  if (!displays.length && !trees.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  let entries = displays.map((d, i) => {
    let tree = trees.find((t) => t.display_addr === d.addr) || { display_addr: d.addr, screens: [] }, objs = [];
    function walk(obj) {
      let c = obj.coords || { x1: 0, y1: 0, x2: 0, y2: 0 };
      objs.push({ addr: obj.addr, x1: c.x1 || 0, y1: c.y1 || 0, x2: c.x2 || 0, y2: c.y2 || 0 }), obj.children?.forEach(walk);
    }
    return tree.screens.forEach(walk), { disp: d, tree, dispObjs: { [d.addr]: objs }, idx: i };
  }), tabBar = el("div", "disp-tab-bar"), contentArea = el("div", "disp-content-area"), tabBtns = [], detailPanel = null, cleanupScene;
  function showDisplay(idx) {
    if (tabBtns.forEach((b, j) => b.classList.toggle("active", j === idx)), cleanupScene)
      cleanupScene();
    resetHL(), contentArea.innerHTML = "", Object.keys(objDataMap).forEach((k) => delete objDataMap[k]);
    let entry = entries[idx], d = entry.disp, chip = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "disp-chip");
      let _e1 = document.createElement("span");
      _e1.setAttribute("class", "disp-addr"), _e1.append(d.addr || "");
      let _e2 = document.createElement("span");
      _e2.setAttribute("class", "disp-res"), _e2.append(d.hor_res + " × " + d.ver_res);
      let _e3 = document.createElement("span");
      return _e3.setAttribute("class", "disp-screens"), _e3.append(d.screen_count + " screens"), _e.append(_e1, _e2, _e3), _e;
    })();
    [d.buf_1, d.buf_2].forEach((b, i) => {
      if (!b)
        return;
      chip.appendChild(badge("buf " + (i + 1) + " " + b.width + "×" + b.height + " " + b.color_format, "blue"));
    });
    let infoBar = el("div", "disp-info-bar");
    infoBar.appendChild(chip), contentArea.appendChild(infoBar);
    let treeView = el("div", "obj-tree-view");
    treeView.appendChild(el("div", "obj-tree-header", "\\uD83C\\uDF33 " + entry.dispObjs[d.addr].length + " objects")), entry.tree.screens.forEach((s) => treeView.appendChild(renderObjTree(s)));
    let view3d = el("div", "obj-3d-view");
    cleanupScene = build3DScene(view3d, [entry.tree], [d], entry.dispObjs), detailPanel = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "obj-detail-view");
      let _e1 = document.createElement("p");
      return _e1.setAttribute("class", "empty"), _e1.append("Select an object to inspect."), _e.append(_e1), _e;
    })();
    let split = (() => {
      let _e = document.createElement("div");
      return _e.setAttribute("class", "obj-split"), _e;
    })();
    split.append(treeView, view3d, detailPanel), contentArea.appendChild(split);
  }
  if (selectedAddr.sub(() => {
    if (selectedAddr.val && detailPanel)
      renderObjDetail(selectedAddr.val, detailPanel);
  }), entries.forEach((entry, i) => {
    let d = entry.disp, btn = el("button", "disp-tab-btn");
    [d.buf_1, d.buf_2].forEach((b) => {
      if (!b?.image_base64)
        return;
      let thumb = (() => {
        let _e = document.createElement("img");
        return _e.setAttribute("class", "disp-tab-thumb"), _e.setAttribute("draggable", "false"), _e;
      })();
      thumb.src = "data:image/png;base64," + b.image_base64, btn.appendChild(thumb);
    }), btn.appendChild(document.createTextNode(d.hor_res + "×" + d.ver_res)), btn.addEventListener("click", () => showDisplay(i)), tabBtns.push(btn), tabBar.appendChild(btn);
  }), body.appendChild(tabBar), body.appendChild(contentArea), entries.length > 0)
    showDisplay(0);
  return panel;
}

// src/builders/sections.ui.ts
function buildSimpleTable(data, key, cls, icon, title, headers, prefix) {
  let items = data[key] || [], { panel, body } = makePanel(cls, icon, title, items.length);
  return body.appendChild(makeTable(headers, items, prefix)), panel;
}
function buildAnimations(data) {
  let items = data.animations || [], { panel, body } = makePanel("panel-animations", "\\uD83C\\uDFAC", "Animations", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return items.forEach((a) => {
    let statusColor = a.status === "paused" ? "yellow" : a.status === "reverse" ? "mauve" : "green", range = a.end_value - a.start_value, ratio = range !== 0 ? (a.current_value - a.start_value) / range : 0;
    body.appendChild(buildCard(a, {
      cardClass: "anim-card",
      anchorPrefix: "anim",
      badges: [{ text: a.status || "running", color: statusColor }],
      content: (info) => {
        info.appendChild(kvPair("callback", a.exec_cb || "-")), info.appendChild(kvPair("duration", a.duration + "ms"));
        let valRow = (() => {
          let _e = document.createElement("div");
          _e.setAttribute("class", "anim-value-row");
          let _e1 = document.createElement("span");
          _e1.setAttribute("class", "anim-val-label"), _e1.append(String(a.start_value));
          let _e2 = document.createElement("span");
          return _e2.setAttribute("class", "anim-val-label"), _e2.append(String(a.end_value)), _e.append(_e1, _e2), _e;
        })();
        valRow.insertBefore(progressBar(ratio, "blue"), valRow.lastChild), info.appendChild(valRow), info.appendChild(el("div", "anim-cur-val", "current: " + a.current_value + "  (" + Math.round(ratio * 100) + "%)")), info.appendChild(kvPair("repeat", a.repeat_cnt === C.INFINITE_REPEAT ? "∞" : String(a.repeat_cnt))), info.appendChild(kvPair("act_time", a.act_time + "ms"));
      }
    }));
  }), panel;
}
function buildTimers(data) {
  let items = data.timers || [], { panel, body } = makePanel("panel-timers", "⏱", "Timers", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return items.forEach((t) => {
    body.appendChild(buildCard(t, {
      cardClass: "timer-card",
      anchorPrefix: "timer",
      badges: [{ text: t.paused ? "paused" : "active", color: t.paused ? "yellow" : "green" }],
      content: (info) => {
        info.classList.add("timer-info-row"), info.appendChild(kvPair("callback", t.timer_cb || "-")), info.appendChild(kvPair("period", t.period + "ms")), info.appendChild(kvPair("frequency", t.frequency || "-")), info.appendChild(kvPair("repeat", t.repeat_count === -1 ? "∞" : String(t.repeat_count))), info.appendChild(kvPair("last_run", String(t.last_run)));
      }
    }));
  }), panel;
}
function buildIndevs(data) {
  let items = data.indevs || [], { panel, body } = makePanel("panel-indevs", "\\uD83D\\uDD79", "Input Devices", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return items.forEach((d) => {
    let card = buildCard(d, {
      cardClass: "indev-card",
      anchorPrefix: "indev",
      badges: [{ text: d.enabled ? "enabled" : "disabled", color: d.enabled ? "green" : "red" }],
      content: (info) => {
        if (info.appendChild(kvPair("read_cb", d.read_cb || "-")), info.appendChild(kvPair("long_press", d.long_press_time + "ms")), info.appendChild(kvPair("scroll_limit", String(d.scroll_limit))), d.display_addr) {
          let row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "display")), row.appendChild(xref(d.display_addr, "disp")), info.appendChild(row);
        }
        if (d.group_addr) {
          let row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "group")), row.appendChild(xref(d.group_addr, "group")), info.appendChild(row);
        }
      }
    }), hdr = card.querySelector(".indev-header"), addrSpan = hdr.firstChild;
    hdr.insertBefore(el("span", "indev-type-name", d.type_name || "unknown"), addrSpan), hdr.insertBefore(el("span", "indev-type-icon", INDEV_ICONS[d.type_name] || "\\uD83D\\uDD79"), hdr.firstChild), body.appendChild(card);
  }), panel;
}
function buildImageCache(data) {
  let entries = data.image_cache || [], { panel, body } = makePanel("panel-img-cache", "\\uD83D\\uDDBC", "Image Cache", entries.length);
  if (!entries.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  let grid = el("div", "cache-grid");
  return entries.forEach((e) => {
    let card = el("div", "cache-entry");
    if (e.entry_addr)
      card.id = "imgcache-" + e.entry_addr;
    if (e.preview_base64) {
      let img = document.createElement("img");
      img.className = "cache-thumb", img.src = "data:image/png;base64," + e.preview_base64, img.alt = "preview", card.appendChild(img);
    }
    let info = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "cache-info");
      let _e1 = document.createElement("div");
      _e1.setAttribute("class", "cache-src"), _e1.append(e.src || "-");
      let _e2 = document.createElement("div");
      return _e2.setAttribute("class", "cache-meta-row"), _e.append(_e1, _e2), _e;
    })();
    if (info.querySelector(".cache-meta-row").append(badge(e.cf || "?", "blue"), el("span", "cache-size-label", e.size || ""), badge("rc=" + e.ref_count, "teal")), e.decoder_name)
      info.appendChild(el("div", "cache-decoder-label", e.decoder_name));
    card.appendChild(info), grid.appendChild(card);
  }), body.appendChild(grid), panel;
}
function buildSubjects(data) {
  let subjects = data.subjects || [], { panel, body } = makePanel("panel-subjects", "\\uD83D\\uDCE1", "Subjects", subjects.length);
  if (!subjects.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return subjects.forEach((s) => {
    let card = el("div", "subject-card");
    if (s.addr)
      card.id = "subject-" + s.addr;
    let hdr = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "subject-header");
      let _e1 = document.createElement("span");
      _e1.setAttribute("class", "subject-addr"), _e1.append(s.addr || "");
      let _e2 = document.createElement("span");
      return _e2.setAttribute("class", "subject-type"), _e2.append(s.type_name), _e.append(_e1, _e2), _e;
    })();
    if (hdr.appendChild(document.createTextNode("  " + (s.observers?.length || 0) + " observers")), card.appendChild(hdr), s.observers?.length) {
      let t = document.createElement("table"), th = t.createTHead().insertRow();
      ["addr", "cb", "target_addr", "for_obj"].forEach((h) => {
        let c = document.createElement("th");
        c.textContent = h, th.appendChild(c);
      });
      let tb = t.createTBody();
      s.observers.forEach((o) => {
        let r = tb.insertRow();
        if (o.addr)
          r.id = "observer-" + o.addr;
        r.insertCell().textContent = o.addr || "-", r.insertCell().textContent = o.cb || "-", xrefCell(r.insertCell(), "target_addr", o.target_addr), r.insertCell().textContent = String(o.for_obj);
      }), card.appendChild(t);
    }
    body.appendChild(card);
  }), panel;
}
function buildGroups(data) {
  let items = data.groups || [], { panel, body } = makePanel("panel-groups", "\\uD83D\\uDC65", "Groups", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return items.forEach((g) => {
    let badges = [{ text: g.obj_count + " objects", color: "blue" }];
    if (g.frozen)
      badges.push({ text: "frozen", color: "yellow" });
    if (g.editing)
      badges.push({ text: "editing", color: "peach" });
    body.appendChild(buildCard(g, {
      cardClass: "group-card",
      anchorPrefix: "group",
      badges,
      content: (info) => {
        if (info.appendChild(kvPair("wrap", String(g.wrap))), g.focused_addr) {
          let row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "focused")), row.appendChild(xref(g.focused_addr, "obj")), info.appendChild(row);
        }
        if (g.member_addrs?.length) {
          let mRow = el("div", "group-members");
          mRow.appendChild(el("span", "kv-label", "members"));
          let mList = el("div", "member-list");
          g.member_addrs.forEach((a) => mList.appendChild(xref(a, "obj"))), mRow.appendChild(mList), info.appendChild(mRow);
        }
      }
    }));
  }), panel;
}
function buildDrawTasks(data) {
  let items = data.draw_tasks || [], { panel, body } = makePanel("panel-draw-tasks", "\\uD83D\\uDCDD", "Draw Tasks", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  return items.forEach((t) => {
    let stateColor = t.state_name === "ready" ? "green" : t.state_name === "queued" ? "yellow" : "mauve";
    body.appendChild(buildCard(t, {
      cardClass: "dtask-card",
      anchorPrefix: "drawtask",
      badges: [{ text: t.type_name || "?", color: "blue" }, { text: t.state_name || "?", color: stateColor }],
      content: (info) => {
        if (t.area)
          info.appendChild(kvPair("area", "(" + t.area.x1 + "," + t.area.y1 + ") → (" + t.area.x2 + "," + t.area.y2 + ")")), info.appendChild(kvPair("size", t.area.x2 - t.area.x1 + " × " + (t.area.y2 - t.area.y1)));
        if (info.appendChild(kvPair("opacity", String(t.opa))), t.preferred_draw_unit_id !== void 0)
          info.appendChild(kvPair("unit_id", String(t.preferred_draw_unit_id)));
      }
    }));
  }), panel;
}
function buildDrawUnits(data) {
  let items = data.draw_units || [], { panel, body } = makePanel("panel-draw-units", "\\uD83C\\uDFA8", "Draw Units", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  let grid = el("div", "unit-grid");
  return items.forEach((u) => {
    let card = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "unit-card");
      let _e1 = document.createElement("div");
      _e1.setAttribute("class", "unit-name"), _e1.append(u.name || "(unnamed)");
      let _e2 = document.createElement("div");
      _e2.setAttribute("class", "unit-idx"), _e2.append("#" + u.idx);
      let _e3 = document.createElement("div");
      return _e3.setAttribute("class", "mono-addr"), _e3.append(u.addr), _e.append(_e1, _e2, _e3), _e;
    })();
    if (u.addr)
      card.id = "drawunit-" + u.addr;
    grid.appendChild(card);
  }), body.appendChild(grid), panel;
}
function buildDecoders(data) {
  let items = data.image_decoders || [], { panel, body } = makePanel("panel-decoders", "\\uD83D\\uDD13", "Decoders", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  let grid = el("div", "decoder-grid");
  return items.forEach((d) => {
    let card = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "decoder-card");
      let _e1 = document.createElement("div");
      _e1.setAttribute("class", "decoder-name"), _e1.append(d.name || "(unnamed)");
      let _e2 = document.createElement("div");
      _e2.setAttribute("class", "decoder-cbs");
      let _e3 = document.createElement("div");
      return _e3.setAttribute("class", "mono-addr"), _e3.append(d.addr), _e.append(_e1, _e2, _e3), _e;
    })();
    if (d.addr)
      card.id = "decoder-" + d.addr;
    let cbs = card.querySelector(".decoder-cbs");
    if (d.info_cb && d.info_cb !== "-")
      cbs.appendChild(badge("info", "blue"));
    if (d.open_cb && d.open_cb !== "-")
      cbs.appendChild(badge("open", "green"));
    if (d.close_cb && d.close_cb !== "-")
      cbs.appendChild(badge("close", "peach"));
    grid.appendChild(card);
  }), body.appendChild(grid), panel;
}
function buildFsDrivers(data) {
  let items = data.fs_drivers || [], { panel, body } = makePanel("panel-fs-drivers", "\\uD83D\\uDCBE", "FS Drivers", items.length);
  if (!items.length)
    return body.appendChild(el("p", "empty", "No entries.")), panel;
  let grid = el("div", "fs-grid");
  return items.forEach((d) => {
    let card = (() => {
      let _e = document.createElement("div");
      _e.setAttribute("class", "fs-card");
      let _e1 = document.createElement("div");
      _e1.setAttribute("class", "fs-letter"), _e1.append(d.letter + ":");
      let _e2 = document.createElement("div");
      _e2.setAttribute("class", "fs-driver-name"), _e2.append(d.driver_name || "(unnamed)");
      let _e3 = document.createElement("div");
      _e3.setAttribute("class", "fs-info");
      let _e4 = document.createElement("div");
      return _e4.setAttribute("class", "fs-cbs"), _e3.append(_e4), _e.append(_e1, _e2, _e3), _e;
    })();
    if (d.addr)
      card.id = "fsdrv-" + d.addr;
    let info = card.querySelector(".fs-info");
    info.insertBefore(kvPair("cache", String(d.cache_size)), info.firstChild);
    let cbs = card.querySelector(".fs-cbs");
    if (d.open_cb && d.open_cb !== "-")
      cbs.appendChild(badge("open", "green"));
    if (d.read_cb && d.read_cb !== "-")
      cbs.appendChild(badge("read", "blue"));
    if (d.write_cb && d.write_cb !== "-")
      cbs.appendChild(badge("write", "peach"));
    if (d.close_cb && d.close_cb !== "-")
      cbs.appendChild(badge("close", "mauve"));
    grid.appendChild(card);
  }), body.appendChild(grid), panel;
}

// src/components/ui-dashboard.ui.ts
class UiDashboard extends BaseComponent {
  static __style = "";
  static __template = () => {
    let _e = document.createElement("div");
    return _e.setAttribute("class", "main"), _e.setAttribute("id", "bento-grid"), _e;
  };
  render() {
    let grid = this.el;
    dashData.sub(() => {
      let data = dashData.val;
      if (!data)
        return;
      setWidgetSpecs(data.widget_specs || {}), grid.innerHTML = "";
      let bento = el("div", "bento"), objCount = countObjects(data.object_trees || []);
      STAT_DEFS.forEach((s) => {
        let val = s.key === "_objects" ? objCount : data[s.key]?.length || 0;
        bento.appendChild(makeStatPanel(s.icon, s.label, val, s.color, s.section));
      }), bento.appendChild(buildDisplayAndTrees(data)), bento.appendChild(buildImageCache(data)), bento.appendChild(buildSimpleTable(data, "image_header_cache", "panel-hdr-cache", "\\uD83D\\uDCCB", "Header Cache", ["entry_addr", "src", "size", "cf", "ref_count", "src_type", "decoder_name"], "imghdr")), bento.appendChild(buildDecoders(data)), bento.appendChild(buildAnimations(data)), bento.appendChild(buildTimers(data)), bento.appendChild(buildIndevs(data)), bento.appendChild(buildGroups(data)), bento.appendChild(buildDrawUnits(data)), bento.appendChild(buildDrawTasks(data)), bento.appendChild(buildSubjects(data)), bento.appendChild(buildFsDrivers(data)), grid.appendChild(bento);
    });
  }
}
customElements.define("ui-dashboard", UiDashboard);

// src/app.ts
document.getElementById("about").innerHTML = `<span>uinspy v${"0.5.0"}</span><span>·</span><span>Built ${"2026-04-21 23:45 GMT+8"}</span><span>·</span><span>${"4b05eb0"}</span><span>·</span><span>${"Canvas2D"}</span><span>·</span><a href="https://github.com/W-Mai/uinspy" target="_blank" rel="noopener">GitHub</a><span>·</span><a href="https://lvgl.io" target="_blank" rel="noopener">LVGL</a><span>·</span><span>MIT</span>`;
</script>
</body>
</html>"""

FILES['lvglgdb/cmds/debugger.py'] = """import argparse

import gdb


class Debugger(gdb.Command):
    \"\"\"Start debugpy server or connect to a debug server, so we can debug python code from IDE like PyCharm/VSCode\"\"\"

    def __init__(self, host="localhost", port=11451):
        self.__host = host
        self.__port = port
        super().__init__("debugger", gdb.COMMAND_USER)

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description=Debugger.__doc__)
        parser.add_argument(
            "--host",
            type=str,
            help="Server listening host",
        )
        parser.add_argument(
            "-p",
            "--port",
            type=int,
            help="Server listening port",
        )
        parser.add_argument(
            "-t",
            "--type",
            choices=["pycharm", "vscode", "eclipse"],
            help="Debugger type",
            required=True,
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.host:
            self.__host = args.host

        if args.port:
            self.__port = args.port

        if args.type == "pycharm":
            self.connect_to_pycharm()
        elif args.type == "vscode":
            self.connect_to_vscode()
        elif args.type == "eclipse":
            self.connect_to_eclipse()

    def connect_to_pycharm(self):
        try:
            import pydevd_pycharm
        except ImportError:
            print("pydevd_pycharm module not found. Please install it using pip.")
            return

        pydevd_pycharm.settrace(
            self.__host, port=self.__port, stdoutToServer=True, stderrToServer=True
        )

    def connect_to_vscode(self):
        try:
            import debugpy
        except ImportError:
            print("debugpy module not found. Please install it using pip.")
            return

        debugpy.listen((self.__host, self.__port))
        debugpy.wait_for_client()

    def connect_to_eclipse(self):
        print("Eclipse is not implemented yet")
"""

FILES['lvglgdb/cmds/display/__init__.py'] = """from .lv_display import DumpDisplayBuf

__all__ = [
    "DumpDisplayBuf",
]
"""

FILES['lvglgdb/cmds/display/lv_display.py'] = """import argparse
import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl import LVDrawBuf


class DumpDisplayBuf(gdb.Command):
    \"\"\"dump display buf to image\"\"\"

    def __init__(self):
        super(DumpDisplayBuf, self).__init__(
            "dump display", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump display draw buffer.")
        parser.add_argument(
            "-p",
            "--prefix",
            type=str,
            default="",
            help="prefix of dump file path",
        )
        parser.add_argument(
            "-f",
            "--format",
            type=str,
            choices=["bmp", "png"],
            default=None,
            help="dump file format (bmp or png)",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        display = curr_inst().disp_default()
        if not display:
            print("Error: Invalid display pointer")
            return
        buffers = {
            "buf_1": display.buf_1,
            "buf_2": display.buf_2,
        }

        for buf_name, buf_ptr in buffers.items():
            if buf_ptr is not None:
                draw_buf = LVDrawBuf(buf_ptr)
                filename = f"{args.prefix}{buf_name}.{args.format.lower() if args.format else 'bmp'}"
                draw_buf.data_dump(filename, args.format)
            else:
                print(f"Warning: {buf_name} buffer is None, skipping.")
"""

FILES['lvglgdb/cmds/draw/__init__.py'] = """from .lv_draw import InfoDrawUnit
from .lv_draw_task import DumpDrawTask

__all__ = [
    "InfoDrawUnit",
    "DumpDrawTask",
]
"""

FILES['lvglgdb/cmds/draw/lv_draw.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.draw.lv_draw_unit import LVDrawUnit
from lvglgdb.lvgl.draw.lv_draw_consts import DRAW_UNIT_TYPE_NAMES


class InfoDrawUnit(gdb.Command):
    \"\"\"dump draw unit info\"\"\"

    def __init__(self):
        super(InfoDrawUnit, self).__init__(
            "info draw_unit", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        for unit in curr_inst().draw_units():
            self._dump_unit(unit)

    def _dump_unit(self, unit: LVDrawUnit):
        name = unit.name
        print(f"Draw Unit: {unit}, Name: {name}")

        type_name = DRAW_UNIT_TYPE_NAMES.get(name, "lv_draw_unit_t")
        casted = unit.cast(type_name, ptr=True)
        if casted is None or not casted.is_ok:
            casted = unit.cast("lv_draw_unit_t", ptr=True)
            if casted is None or not casted.is_ok:
                print(f"  (corrupted: {casted})")
                return
        deref = casted.dereference()
        if not deref.is_ok:
            print(f"  (corrupted: {deref})")
            return
        print(deref.format_string(pretty_structs=True, symbols=True))
"""

FILES['lvglgdb/cmds/draw/lv_draw_task.py'] = """import gdb

from lvglgdb.value import Value
from lvglgdb.lvgl.draw.lv_draw_task import LVDrawTask
from lvglgdb.lvgl.formatter import print_spec_table


class DumpDrawTask(gdb.Command):
    \"\"\"dump draw tasks from a layer\"\"\"

    def __init__(self):
        super(DumpDrawTask, self).__init__(
            "dump draw_task", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        if not args.strip():
            print("Usage: dump draw_task <layer_expression>")
            return

        layer = Value(gdb.parse_and_eval(args.strip()))
        task_head = layer.draw_task_head
        if not int(task_head):
            print("No draw tasks on this layer.")
            return
        print_spec_table(
            LVDrawTask.snapshots(LVDrawTask(task_head)),
        )
"""

FILES['lvglgdb/cmds/drivers/__init__.py'] = """from .nuttx import Lvglobal

__all__ = [
    "Lvglobal",
]
"""

FILES['lvglgdb/cmds/drivers/nuttx/__init__.py'] = """from .nuttx import Lvglobal

__all__ = [
    "Lvglobal",
]
"""

FILES['lvglgdb/cmds/drivers/nuttx/nuttx.py'] = """# GDB script to get lvgl global pointer in NuttX.

import argparse
import sys

import gdb


class Lvglobal(gdb.Command):
    \"\"\"Set which lvgl instance to inspect by finding lv_global pointer in task TLS data.\"\"\"

    def __init__(self):
        super(Lvglobal, self).__init__("lvglobal", gdb.COMMAND_USER)

    def set_lvgl_instance(self, inst):
        from lvglgdb.lvgl import curr_inst

        curr_inst().reset()
        curr_inst().ensure_init(inst)

    def invoke(self, arg, from_tty):
        parser = argparse.ArgumentParser(description=self.__doc__)
        parser.add_argument("-p", "--pid", type=int, help="Optional process ID")
        try:
            args = parser.parse_args(gdb.string_to_argv(arg))
        except SystemExit:
            return
        try:
            from nxgdb import utils
        except ImportError:
            print("nxgdb is not installed, can't find lvgl global pointer.")
            return

        lv_global = utils.gdb_eval_or_none("lv_global")
        if lv_global:
            print(f"Found single instance lv_global@{hex(lv_global.address)}")
            self.set_lvgl_instance(lv_global)
            return

        # find the lvgl global pointer in tls
        if not args.pid:
            print("LVGL is in multi-process mode; please provide --pid.")
            return
        lv_key = utils.gdb_eval_or_none("lv_nuttx_tlskey")
        if lv_key is None:
            lv_key = utils.gdb_eval_or_none("lv_global_default::index")
        if lv_key is None:
            print("Can't find lvgl tls key in multi-process mode.")
            return
        lv_global = utils.get_task_tls(args.pid, lv_key)
        if lv_global:
            print(f"Found lv_global@{hex(lv_global)}")
            self.set_lvgl_instance(lv_global)
        else:
            print(f"\\nCan't find lv_global with tlskey@{hex(lv_key)}.")
"""

FILES['lvglgdb/cmds/misc/__init__.py'] = """from .lv_style import InfoStyle
from .lv_cache import DumpCache, CheckPrefix, CheckCache
from .lv_anim import DumpAnim
from .lv_timer import DumpTimer
from .lv_image_decoder import DumpImageDecoder
from .lv_fs import DumpFsDrv
from .lv_version import InfoVersion

__all__ = [
    "InfoStyle",
    "DumpCache",
    "CheckPrefix",
    "CheckCache",
    "DumpAnim",
    "DumpTimer",
    "DumpImageDecoder",
    "DumpFsDrv",
    "InfoVersion",
]
"""

FILES['lvglgdb/cmds/misc/lv_anim.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_anim import LVAnim
from lvglgdb.lvgl.formatter import print_info, print_spec_table


class DumpAnim(gdb.Command):
    \"\"\"dump all active animations\"\"\"

    def __init__(self):
        super(DumpAnim, self).__init__(
            "dump anim", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        anims = list(curr_inst().anims())
        if not anims:
            print("No active animations.")
            return

        if args.strip() == "--detail":
            for anim in anims:
                print_info(anim.snapshot())
                print()
        else:
            print_spec_table(LVAnim.snapshots(anims))
"""

FILES['lvglgdb/cmds/misc/lv_cache.py'] = """import argparse
import gdb


class DumpCache(gdb.Command):
    \"\"\"dump cache info for specified cache\"\"\"

    def __init__(self):
        super(DumpCache, self).__init__(
            "dump cache", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl cache info.")
        parser.add_argument(
            "cache",
            type=str,
            choices=["image", "image_header"],
            default="image",
            help="cache to dump.",
        )

        from lvglgdb import curr_inst

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        cache = None
        if args.cache == "image":
            cache = curr_inst().image_cache()
        elif args.cache == "image_header":
            cache = curr_inst().image_header_cache()

        if not cache:
            print("Invalid cache: ", args.cache)
            return

        from lvglgdb.lvgl.formatter import print_info, print_spec_table

        # Print cache-level info
        snap = cache.snapshot()
        print_info(snap)

        # Print cache entries
        snaps = cache.snapshots()
        extra_fields = getattr(cache, "_last_extra_fields", [])

        col_align = {"src": "l", "type": "c"}

        extra_columns = ["entry"] + extra_fields if extra_fields else ["entry"]

        def _extra_row(d):
            extras = d.get("extra_fields", {})
            extra_vals = [extras.get(f, "") for f in extra_fields]
            return [d["entry_addr"]] + extra_vals

        print_spec_table(snaps,
                         align="r", numbered=False, col_align=col_align,
                         extra_columns=extra_columns,
                         extra_row_fn=_extra_row)


class CheckPrefix(gdb.Command):
    \"\"\"prefix command for check subcommands\"\"\"

    def __init__(self):
        super(CheckPrefix, self).__init__(
            "check", gdb.COMMAND_USER, gdb.COMPLETE_NONE, True
        )

    def invoke(self, args, from_tty):
        gdb.execute("help check")


class CheckCache(gdb.Command):
    \"\"\"run sanity check on specified cache\"\"\"

    def __init__(self):
        super(CheckCache, self).__init__(
            "check cache", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Run cache sanity check.")
        parser.add_argument(
            "cache",
            type=str,
            choices=["image", "image_header"],
            default="image",
            help="cache to check.",
        )

        from lvglgdb import curr_inst

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        cache = None
        if args.cache == "image":
            cache = curr_inst().image_cache()
        elif args.cache == "image_header":
            cache = curr_inst().image_header_cache()

        if not cache:
            print("Invalid cache: ", args.cache)
            return

        cache.sanity_check()
"""

FILES['lvglgdb/cmds/misc/lv_fs.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_fs import LVFsDrv
from lvglgdb.lvgl.formatter import print_spec_table


class DumpFsDrv(gdb.Command):
    \"\"\"dump all registered filesystem drivers\"\"\"

    def __init__(self):
        super(DumpFsDrv, self).__init__(
            "dump fs_drv", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        drivers = curr_inst().fs_drivers()
        snaps = LVFsDrv.snapshots(drivers)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVFsDrv._DISPLAY_SPEC["empty_msg"])
"""

FILES['lvglgdb/cmds/misc/lv_image_decoder.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_image_decoder import LVImageDecoder
from lvglgdb.lvgl.formatter import print_spec_table


class DumpImageDecoder(gdb.Command):
    \"\"\"dump all registered image decoders\"\"\"

    def __init__(self):
        super(DumpImageDecoder, self).__init__(
            "dump image_decoder", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        decoders = curr_inst().image_decoders()
        snaps = LVImageDecoder.snapshots(decoders)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVImageDecoder._DISPLAY_SPEC["empty_msg"])
"""

FILES['lvglgdb/cmds/misc/lv_style.py'] = """import argparse
import gdb

from lvglgdb.lvgl import LVStyle, dump_obj_styles
from lvglgdb.lvgl.formatter import print_table


def _style_row_fn(_i, d):
    \"\"\"Format a style property row with optional ANSI color block.\"\"\"
    value_str = d["value_str"]
    color_rgb = d.get("color_rgb")
    if color_rgb:
        r, g, b = color_rgb["r"], color_rgb["g"], color_rgb["b"]
        value_str = f"{value_str} \\033[48;2;{r};{g};{b}m  \\033[0m"
    return [d["prop_name"], value_str]


def print_style_props(entries):
    \"\"\"Print style properties as a 2-column table with optional ANSI color.\"\"\"
    print_table(
        entries, ["prop", "value"], _style_row_fn,
        "Empty style.", align="l", numbered=False,
    )


class InfoStyle(gdb.Command):
    \"\"\"Dump style properties. Default: single lv_style_t. Use --obj for object styles.\"\"\"

    def __init__(self):
        super(InfoStyle, self).__init__(
            "info style", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl style properties.")
        parser.add_argument(
            "style",
            type=str,
            nargs="?",
            help="lv_style_t variable to inspect.",
        )
        parser.add_argument(
            "--obj",
            type=str,
            default=None,
            help="lv_obj_t variable to inspect all styles.",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.obj:
            obj = gdb.parse_and_eval(args.obj)
            if not obj:
                print("Invalid obj:", args.obj)
                return
            dump_obj_styles(obj)
        elif args.style:
            style = gdb.parse_and_eval(args.style)
            if not style:
                print("Invalid style:", args.style)
                return
            print_style_props(LVStyle(style).snapshots())
        else:
            print("Usage: info style <style_var> or info style --obj <obj_var>")
"""

FILES['lvglgdb/cmds/misc/lv_timer.py'] = """import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_timer import LVTimer
from lvglgdb.lvgl.formatter import print_spec_table


class DumpTimer(gdb.Command):
    \"\"\"dump all active timers\"\"\"

    def __init__(self):
        super(DumpTimer, self).__init__(
            "dump timer", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        timers = curr_inst().timers()
        snaps = LVTimer.snapshots(timers)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVTimer._DISPLAY_SPEC["empty_msg"])
"""

FILES['lvglgdb/cmds/misc/lv_version.py'] = """import os
import re

import gdb

import lvglgdb


def _call(expression):
    try:
        return gdb.parse_and_eval(expression)
    except gdb.error:
        return None


_VERSION_MACRO = re.compile(
    r"#define\\s+LVGL_VERSION_(MAJOR|MINOR|PATCH|INFO)\\s+(\\S+)")


def version_from_sources():
    \"\"\"The version read from the lv_version.h the target was built against.

    Neither way of asking the binary is dependable. lv_version_major() is
    `static inline`, so it is dropped unless something calls it, and the macros
    need -g3 *and* a stop inside a translation unit that included lv_version.h -
    lv_timer.c, where a breakpoint on lv_timer_handler lands, does not.

    DWARF does record the path of every source file, so the LVGL tree can be
    found through any of its symbols and the version header read from disk.
    Returns (major, minor, patch, info) or None.
    \"\"\"
    for symbol in ("lv_obj_create", "lv_timer_handler", "lv_init"):
        try:
            found = gdb.lookup_global_symbol(symbol)
            if found is None or found.symtab is None:
                continue
            # DWARF records whatever separator the compiler used, which is not
            # always the host's: a MinGW build writes forward slashes.
            path = found.symtab.fullname().replace("\\\\", "/")
            marker = "/src/"
            if marker not in path:
                continue
            root = path[:path.rindex(marker)]
            # Since 9.6 the public headers live in include/lvgl/ and the
            # top-level lv_version.h is a deprecated wrapper that only includes
            # the real one, so the defines are not in it. Read that one first
            # and keep the top-level file for the releases that still have it.
            for header in (os.path.join(root, "include", "lvgl", "lv_version.h"),
                           os.path.join(root, "lv_version.h")):
                if not os.path.exists(header):
                    continue
                with open(header) as f:
                    macros = dict(_VERSION_MACRO.findall(f.read()))
                if not {"MAJOR", "MINOR", "PATCH"} <= set(macros):
                    continue
                return (int(macros["MAJOR"]), int(macros["MINOR"]),
                        int(macros["PATCH"]), macros.get("INFO", '""').strip('"'))
        except (gdb.error, OSError, ValueError):
            continue
    return None


def lvgl_version():
    \"\"\"The LVGL version of the target, as (major, minor, patch, info).

    lv_version_major() and friends in lvgl.h are the intended way to ask, with
    the version macros as a second try. Both depend on how the target was
    built, so lv_version.h itself is the third and most dependable one.
    Fields that cannot be read come back as None.
    \"\"\"
    parts = []
    for name in ("major", "minor", "patch"):
        value = _call(f"lv_version_{name}()")
        if value is None:
            value = _call(f"LVGL_VERSION_{name.upper()}")
        parts.append(int(value) if value is not None else None)

    info = _call("lv_version_info()") or _call("LVGL_VERSION_INFO")
    try:
        info = info.string() if info is not None else None
    except gdb.error:
        info = None

    # `info` counts as missing too: a build that hides it loses the "-dev" that
    # tells a development version from a release.
    if any(p is None for p in parts) or info is None:
        from_sources = version_from_sources()
        if from_sources is not None:
            return from_sources

    return (*parts, info)


def version_string():
    \"\"\""9.6.0-dev", or as much of it as the target makes readable.\"\"\"
    major, minor, patch, info = lvgl_version()
    if major is None:
        return ("unknown: lv_version_major() was inlined away, the version "
                "macros need a -g3 build, and LVGL's sources are not on disk")
    numbers = ".".join("?" if p is None else str(p) for p in (major, minor, patch))
    return numbers + (f"-{info}" if info else "")


class InfoVersion(gdb.Command):
    \"\"\"print the LVGL version of the target, and this plugin's version\"\"\"

    def __init__(self):
        super(InfoVersion, self).__init__(
            "info lvgl_version", gdb.COMMAND_USER, gdb.COMPLETE_NONE
        )

    def invoke(self, args, from_tty):
        print(f"LVGL:    {version_string()}")
        print(f"lvglgdb: {lvglgdb.__version__}")
"""

FILES['lvglgdb/lvgl/__init__.py'] = """from .core import (
    LVObject,
    ObjStyle,
    curr_inst,
    dump_obj_info,
    dump_obj_styles,
    LVIndev,
    INDEV_TYPE_NAMES,
    LVGroup,
    LVObjClass,
    OBJ_FLAG_NAMES,
    decode_obj_flags,
    OBJ_STATE_NAMES,
    decode_obj_states,
    LVSubject,
    LVObserver,
    SUBJECT_TYPE_NAMES,
)
from .display import LVDisplay
from .draw import (
    LVDrawBuf,
    LVDrawTask,
    LVDrawUnit,
    DRAW_TASK_TYPE_NAMES,
    DRAW_TASK_STATE_NAMES,
    DRAW_UNIT_TYPE_NAMES,
)
from .misc import (
    LVList,
    LVStyle,
    StyleEntry,
    style_prop_name,
    decode_selector,
    format_style_value,
    LVRedBlackTree,
    LVCache,
    LVCacheEntry,
    LVCacheLRURB,
    LVCacheLRURBIterator,
    LVCacheIteratorBase,
    LVImageCache,
    LVImageHeaderCache,
    create_cache_iterator,
    LVArray,
    LVEvent,
    LVEventDsc,
    LVEventList,
    event_code_name,
    EVENT_CODE_NAMES,
    LVAnim,
    LVTimer,
    LVImageDecoder,
    LVFsDrv,
    format_coord,
)
from .snapshot import Snapshot
from .data_utils import fmt_cb, ptr_or_none
from . import formatter

__all__ = [
    "LVObject",
    "ObjStyle",
    "LVDisplay",
    "LVDrawBuf",
    "LVDrawTask",
    "DRAW_TASK_TYPE_NAMES",
    "DRAW_TASK_STATE_NAMES",
    "DRAW_UNIT_TYPE_NAMES",
    "LVDrawUnit",
    "curr_inst",
    "LVList",
    "LVStyle",
    "StyleEntry",
    "dump_obj_styles",
    "dump_obj_info",
    "style_prop_name",
    "decode_selector",
    "format_style_value",
    "LVRedBlackTree",
    "LVCache",
    "LVCacheEntry",
    "LVCacheLRURB",
    "LVCacheLRURBIterator",
    "LVCacheIteratorBase",
    "LVImageCache",
    "LVImageHeaderCache",
    "create_cache_iterator",
    "LVArray",
    "LVEvent",
    "LVEventDsc",
    "LVEventList",
    "event_code_name",
    "EVENT_CODE_NAMES",
    "LVAnim",
    "LVTimer",
    "LVImageDecoder",
    "LVFsDrv",
    "format_coord",
    "LVIndev",
    "INDEV_TYPE_NAMES",
    "LVGroup",
    "LVObjClass",
    "OBJ_FLAG_NAMES",
    "decode_obj_flags",
    "OBJ_STATE_NAMES",
    "decode_obj_states",
    "LVSubject",
    "LVObserver",
    "SUBJECT_TYPE_NAMES",
    "Snapshot",
    "fmt_cb",
    "ptr_or_none",
    "formatter",
]
"""

FILES['lvglgdb/lvgl/core/__init__.py'] = """from .lv_obj import LVObject, ObjStyle, dump_obj_info, dump_obj_styles
from .lv_global import curr_inst
from .lv_indev import LVIndev, INDEV_TYPE_NAMES
from .lv_group import LVGroup
from .lv_obj_class import LVObjClass
from .lv_obj_flag_consts import OBJ_FLAG_NAMES, decode_obj_flags
from .lv_obj_state_consts import OBJ_STATE_NAMES, decode_obj_states
from .lv_observer import LVSubject, LVObserver
from .lv_observer_consts import SUBJECT_TYPE_NAMES

__all__ = [
    "LVObject",
    "ObjStyle",
    "curr_inst",
    "dump_obj_info",
    "dump_obj_styles",
    "LVIndev",
    "INDEV_TYPE_NAMES",
    "LVGroup",
    "LVObjClass",
    "OBJ_FLAG_NAMES",
    "decode_obj_flags",
    "OBJ_STATE_NAMES",
    "decode_obj_states",
    "LVSubject",
    "LVObserver",
    "SUBJECT_TYPE_NAMES",
]
"""

FILES['lvglgdb/lvgl/core/lv_global.py'] = """import gdb
from typing import Optional, Union, Iterator, TYPE_CHECKING

from lvglgdb.value import Value
from ..misc.lv_ll import LVList

\"\"\"
LVGL global instance

Note:
    lv_global is the global instance of LVGL.
    It will contain other components like display, draw unit, etc.
    So if you want add a new component, please add it in this class.
    And just import your component when you need it. Not expose to the whole file.
\"\"\"

__all__ = ["curr_inst"]

\"\"\"
Type hint for IDE, just imported at typechecking phase not in runtime.
\"\"\"
if TYPE_CHECKING:
    from ..display.lv_display import LVDisplay


class LVGL:
    \"\"\"LVGL instance\"\"\"

    def __init__(self, lv_global: Value):
        self.lv_global = lv_global.cast("lv_global_t", ptr=True)

    def displays(self) -> "Iterator[LVDisplay]":
        ll = self.lv_global.disp_ll
        # disp_ll is an inline lv_ll_t, not a pointer. Some GDB builds cannot
        # convert an aggregate to a boolean, and Value.__bool__ answers False
        # when that conversion fails - which would report no displays at all.
        # The list is empty exactly when it has no head.
        if not ll.head:
            return

        from ..display.lv_display import LVDisplay

        for disp in LVList(ll, "lv_display_t"):
            yield LVDisplay(disp)

    def disp_default(self):
        from ..display.lv_display import LVDisplay

        disp_default = self.lv_global.disp_default
        return LVDisplay(disp_default) if disp_default else None

    def screen_active(self):
        disp = self.lv_global.disp_default
        return disp.act_scr if disp else None

    def draw_units(self):
        from ..draw.lv_draw_unit import LVDrawUnit

        head = self.lv_global.draw_info.unit_head
        if head:
            yield from LVDrawUnit(head)

    def image_cache(self):
        from ..misc.lv_image_cache import LVImageCache

        return LVImageCache(self.lv_global.img_cache)

    def anims(self):
        from ..misc.lv_anim import LVAnim

        for anim in LVList(self.lv_global.anim_state.anim_ll, "lv_anim_t"):
            yield LVAnim(anim)

    def timers(self):
        from ..misc.lv_timer import LVTimer

        for timer in LVList(self.lv_global.timer_state.timer_ll, "lv_timer_t"):
            yield LVTimer(timer)

    def groups(self):
        from .lv_group import LVGroup

        for group in LVList(self.lv_global.group_ll, "lv_group_t"):
            yield LVGroup(group)

    def indevs(self):
        from .lv_indev import LVIndev

        for indev in LVList(self.lv_global.indev_ll, "lv_indev_t"):
            yield LVIndev(indev)

    def image_decoders(self):
        from ..misc.lv_image_decoder import LVImageDecoder

        for dec in LVList(self.lv_global.img_decoder_ll, "lv_image_decoder_t"):
            yield LVImageDecoder(dec)

    def fs_drivers(self):
        from ..misc.lv_fs import LVFsDrv

        # fsdrv_ll stores lv_fs_drv_t* pointers (not inline structs)
        pp_type = gdb.lookup_type("lv_fs_drv_t").pointer().pointer()
        for drv_pp in LVList(self.lv_global.fsdrv_ll, pp_type):
            yield LVFsDrv(drv_pp.dereference())

    def image_header_cache(self):
        from ..misc.lv_image_header_cache import LVImageHeaderCache

        return LVImageHeaderCache(self.lv_global.img_header_cache)


class _LVGLSingleton:
    __slots__ = ("_lvgl", "_ready")

    def __init__(self) -> None:
        self._lvgl: Optional[object] = None
        self._ready = False

    def ensure_init(self, lv_global: Union[gdb.Value, Value, None] = None) -> bool:
        if self._ready:
            return True

        if lv_global is None:
            try:
                lv_global = Value(gdb.parse_and_eval("lv_global").address)
            except gdb.error as e:
                print(f"Failed to get lv_global: {e}")
                return False
        elif not isinstance(lv_global, Value):
            lv_global = Value(lv_global).cast("lv_global_t", ptr=True)

        if not lv_global.inited:
            print(
                "\\x1b[31mlvgl is not initialized yet. "
                "Please call `ensure_init()` later.\\x1b[0m"
            )
            return False

        self._lvgl = LVGL(lv_global)
        self._ready = True
        return True

    def __getattr__(self, name: str):
        if not self._ready and not self.ensure_init():
            raise RuntimeError("LVGL singleton not ready")
        return getattr(self._lvgl, name)

    def reset(self) -> None:
        self._lvgl = None
        self._ready = False


__curr_inst = _LVGLSingleton()


def curr_inst() -> _LVGLSingleton:
    \"\"\"Get the global instance of LVGL\"\"\"
    return __curr_inst
"""

FILES['lvglgdb/lvgl/core/lv_group.py'] = """import gdb

from lvglgdb.value import Value, ValueInput
from ..misc.lv_ll import LVList


class LVGroup(Value):
    \"\"\"LVGL focus group wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("objects", "obj_count"),
            ("frozen", "frozen"),
            ("editing", "editing"),
            ("wrap", "wrap"),
            ("focused", "focused"),
        ],
        "table": [],
        "empty_msg": "No focus groups.",
    }

    def __init__(self, group: ValueInput):
        super().__init__(Value.normalize(group, "lv_group_t"))

    @property
    def frozen(self) -> bool:
        return bool(int(self.super_value("frozen")))

    @property
    def editing(self) -> bool:
        return bool(int(self.super_value("editing")))

    @property
    def wrap(self) -> bool:
        return bool(int(self.super_value("wrap")))

    @property
    def obj_focus(self) -> Value:
        focus_pp = self.super_value("obj_focus")
        if not focus_pp:
            return None
        return focus_pp.dereference()

    @property
    def focus_cb(self) -> Value:
        return self.super_value("focus_cb")

    @property
    def edge_cb(self) -> Value:
        return self.super_value("edge_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def refocus_policy(self) -> bool:
        return bool(int(self.super_value("refocus_policy")))

    @property
    def obj_count(self) -> int:
        return LVList(self.obj_ll, "lv_obj_t").len

    def _members(self):
        \"\"\"The objects in the group.

        lv_group_init() sizes obj_ll for `lv_obj_t *`, so each node holds a
        pointer to the object rather than the object itself. Reading the node as
        an lv_obj_t would report the list's own memory as a widget.
        \"\"\"
        obj_ptr_t = gdb.lookup_type("lv_obj_t").pointer()
        for node in LVList(self.obj_ll):
            # The node's payload is the pointer, so read through it the same way
            # LVList reads its own next/prev links.
            yield Value(int(node)).cast(obj_ptr_t, ptr=True).dereference()

    def __iter__(self):
        from .lv_obj import LVObject

        for obj_ptr in self._members():
            yield LVObject(obj_ptr)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        focus = self.obj_focus
        if focus:
            from .lv_obj import LVObject

            focus_obj = LVObject(focus)
            focus_str = f"{focus_obj.class_name}@{int(focus):x}"
            focused_addr = hex(int(focus))
        else:
            focus_str = "(none)"
            focused_addr = None

        member_addrs = []
        for obj_ptr in self._members():
            addr = int(obj_ptr)
            if addr:
                member_addrs.append(hex(addr))

        d = {
            "addr": hex(int(self)),
            "obj_count": self.obj_count,
            "frozen": self.frozen,
            "editing": self.editing,
            "wrap": self.wrap,
            "focused": focus_str,
            "focused_addr": focused_addr,
            "member_addrs": member_addrs,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(groups):
        return [group.snapshot() for group in groups]
"""

FILES['lvglgdb/lvgl/core/lv_indev.py'] = """from lvglgdb.value import Value, ValueInput
from .lv_indev_consts import INDEV_TYPE_NAMES


class LVIndev(Value):
    \"\"\"LVGL input device wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("type", "type_name"),
            ("enabled", "enabled"),
            ("state", "state"),
            ("read_cb", "read_cb"),
            ("long_press_time", "long_press_time"),
            ("scroll_limit", "scroll_limit"),
            ("group", "group"),
        ],
        "table": [],
        "empty_msg": "No input devices.",
    }

    def __init__(self, indev: ValueInput):
        super().__init__(Value.normalize(indev, "lv_indev_t"))

    @property
    def type(self) -> int:
        return int(self.super_value("type"))

    @property
    def type_name(self) -> str:
        return INDEV_TYPE_NAMES.get(self.type, f"UNKNOWN({self.type})")

    @property
    def state(self) -> int:
        return int(self.super_value("state"))

    @property
    def enabled(self) -> bool:
        return bool(int(self.super_value("enabled")))

    @property
    def long_press_time(self) -> int:
        return int(self.super_value("long_press_time"))

    @property
    def scroll_limit(self) -> int:
        return int(self.super_value("scroll_limit"))

    @property
    def scroll_throw(self) -> int:
        return int(self.super_value("scroll_throw"))

    @property
    def long_press_repeat_time(self) -> int:
        return int(self.super_value("long_press_repeat_time"))

    @property
    def read_cb(self) -> Value:
        return self.super_value("read_cb")

    @property
    def read_timer(self) -> Value:
        return self.super_value("read_timer")

    @property
    def disp(self) -> Value:
        return self.super_value("disp")

    @property
    def group(self) -> Value:
        return self.super_value("group")

    @property
    def cursor(self) -> Value:
        return self.super_value("cursor")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def driver_data(self) -> Value:
        return self.super_value("driver_data")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        grp = int(self.group)
        grp_str = f"0x{grp:x}" if grp else "-"
        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "enabled": self.enabled,
            "state": self.state,
            "read_cb": fmt_cb(self.read_cb),
            "long_press_time": self.long_press_time,
            "scroll_limit": self.scroll_limit,
            "group": grp_str,
            "display_addr": ptr_or_none(self.disp),
            "group_addr": ptr_or_none(self.group),
            "read_timer_addr": ptr_or_none(self.read_timer),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(indevs):
        return [indev.snapshot() for indev in indevs]
"""

FILES['lvglgdb/lvgl/core/lv_indev_consts.py'] = """\"\"\"
Auto-generated indev constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

INDEV_TYPE_NAMES = {
    0: "NONE",
    1: "POINTER",
    2: "KEYPAD",
    3: "BUTTON",
    4: "ENCODER",
}
"""

FILES['lvglgdb/lvgl/core/lv_obj.py'] = """from lvglgdb.value import Value, ValueInput
from lvglgdb.lvgl.misc.lv_style import LVStyle, decode_selector


class ObjStyle:
    \"\"\"A single style slot from obj->styles[], wrapping LVStyle.\"\"\"

    def __init__(self, index: int, selector: int, flags: list, style: LVStyle):
        self.index = index
        self.selector = selector
        self.flags = flags
        self.style = style

    @property
    def selector_str(self) -> str:
        return decode_selector(self.selector)

    @property
    def flags_str(self) -> str:
        return ",".join(self.flags) if self.flags else "-"

    def __iter__(self):
        return iter(self.style)

    def __len__(self):
        return len(list(self.style))

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        props = [s.as_dict() for s in self.style.snapshots()]
        d = {
            "index": self.index,
            "selector": self.selector,
            "selector_str": self.selector_str,
            "flags_str": self.flags_str,
            "properties": props,
        }
        return Snapshot(d, source=self)


class LVObject(Value):
    \"\"\"LVGL object\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: (
                f"{d['class_name']}@{d['addr']}"
                f" {d['coords']['x1']},{d['coords']['y1']},"
                f"{d['coords']['x2']},{d['coords']['y2']}"
            )),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, obj: ValueInput):
        super().__init__(Value.normalize(obj, "lv_obj_t"))

    @property
    def obj_class(self):
        from .lv_obj_class import LVObjClass

        class_p = self.super_value("class_p")
        if not class_p:
            return None
        return LVObjClass(class_p)

    @property
    def class_name(self):
        cls = self.obj_class
        if not cls:
            return "(no class)"
        return cls.name

    @property
    def x1(self):
        return int(self.coords.x1)

    @property
    def y1(self):
        return int(self.coords.y1)

    @property
    def x2(self):
        return int(self.coords.x2)

    @property
    def y2(self):
        return int(self.coords.y2)

    @property
    def flags_raw(self) -> int:
        \"\"\"Return raw flags bitmask, 0 if corrupted.

        Up to LVGL 9.5 this was a single `flags` bitmask. Since 9.6 each flag
        is its own bitfield named after it, so the mask is rebuilt from them.
        Without this every object reports no flags at all, because safe_field()
        cannot tell a missing field from a clear one.
        \"\"\"
        from .lv_obj_flag_consts import OBJ_FLAG_NAMES

        raw = self.safe_field("flags")
        if raw is not None:
            return int(raw)

        value = 0
        for bit, name in OBJ_FLAG_NAMES.items():
            # Composite names (SCROLL_CHAIN) have no field of their own.
            field = self.safe_field(name.lower())
            if field is not None and int(field):
                value |= bit
        return value | self._user_flags_mask()

    def _user_flags_mask(self) -> int:
        \"\"\"The eight flags 9.6 keeps in spec_attr->user_flags, as a flag mask.

        These have no bitfield of their own, so the rebuild above misses all of
        them - and LAYOUT_1 is LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, so losing it costs
        a flex layout its wrapping.
        \"\"\"
        from .lv_obj_flag_consts import OBJ_FLAG_NAMES

        raw = self._get_spec_int("user_flags")
        if not raw:
            return 0

        # The order lv_obj_add_flag() writes them in, from bit 0 up.
        names = ("LAYOUT_1", "LAYOUT_2", "WIDGET_1", "WIDGET_2",
                 "USER_1", "USER_2", "USER_3", "USER_4")
        bit_of = {name: bit for bit, name in OBJ_FLAG_NAMES.items()}
        value = 0
        for index, name in enumerate(names):
            if raw & (1 << index) and name in bit_of:
                value |= bit_of[name]
        return value

    @property
    def flags_list(self) -> list[str]:
        \"\"\"Return decoded flag names.\"\"\"
        from .lv_obj_flag_consts import decode_obj_flags
        return decode_obj_flags(self.flags_raw)

    @property
    def state_raw(self) -> int:
        return int(self.safe_field("state", 0))

    @property
    def state_list(self) -> list[str]:
        from .lv_obj_state_consts import decode_obj_states
        return decode_obj_states(self.state_raw)

    @property
    def child_cnt(self) -> int:
        \"\"\"Return child count, 0 if corrupted.\"\"\"
        if not self.spec_attr:
            return 0
        cnt = self.spec_attr.super_value("child_cnt")
        if not cnt.is_ok:
            return 0
        return int(cnt)

    @property
    def event_list(self):
        \"\"\"Get event list from obj->spec_attr->event_list.\"\"\"
        from ..misc.lv_event import LVEventList

        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        return LVEventList(spec.event_list)

    @property
    def children(self):
        if not self.spec_attr:
            return
        for i in range(self.child_cnt):
            yield LVObject(self.spec_attr.children[i].read_value())

    @property
    def obj_styles(self):
        \"\"\"Yield ObjStyle for each entry in obj->styles[].\"\"\"
        count = int(self.style_cnt)
        if count == 0:
            return
        styles_arr = self.super_value("styles")
        for i in range(count):
            raw = styles_arr[i]
            flags = []
            raw_val = Value(raw)
            for flag_name in ("is_local", "is_trans", "is_theme", "is_disabled"):
                if raw_val.safe_field(flag_name, False, bool):
                    flags.append(flag_name.replace("is_", ""))
            yield ObjStyle(i, int(raw.selector), flags, LVStyle(raw.style))

    @property
    def styles(self):
        \"\"\"Yield StyleEntry for all style properties across all slots.\"\"\"
        for obj_style in self.obj_styles:
            yield from obj_style

    def get_child(self, index: int):
        return self.spec_attr.children[index] if self.spec_attr else None

    def snapshot(self, include_children=False, include_styles=False):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import ptr_or_none

        d = Snapshot.safe_fields(self, [
            ("addr", lambda s: hex(int(s))),
            ("class_name", lambda s: s.class_name, "(corrupted)"),
            ("coords", lambda s: {
                "x1": s.x1, "y1": s.y1, "x2": s.x2, "y2": s.y2,
            }, {"x1": 0, "y1": 0, "x2": 0, "y2": 0}),
            ("child_count", lambda s: s.child_cnt, 0),
            ("style_count", lambda s: int(s.style_cnt), 0),
            ("flags", lambda s: s.flags_raw, 0),
            ("flags_list", lambda s: s.flags_list, []),
            ("state", lambda s: s.state_raw, 0),
            ("state_list", lambda s: s.state_list, []),
            ("parent_addr", lambda s: ptr_or_none(s.super_value("parent"))),
            ("group_addr", lambda s: s._get_group_addr()),
            ("user_data", lambda s: ptr_or_none(s.safe_field("user_data"))),
            ("layout_inv", lambda s: bool(s.safe_field("layout_inv", 0, int)), False),
            ("w_layout", lambda s: bool(s.safe_field("w_layout", 0, int)), False),
            ("h_layout", lambda s: bool(s.safe_field("h_layout", 0, int)), False),
            ("is_deleting", lambda s: bool(s.safe_field("is_deleting", 0, int)), False),
            ("rendered", lambda s: bool(s.safe_field("rendered", 0, int)), False),
            ("skip_trans", lambda s: bool(s.safe_field("skip_trans", 0, int)), False),
            ("scroll", lambda s: s._get_scroll()),
            ("ext_click_pad", lambda s: s._get_spec_int("ext_click_pad")),
            ("ext_draw_size", lambda s: s._get_spec_int("ext_draw_size")),
            ("scrollbar_mode", lambda s: s._get_spec_int("scrollbar_mode")),
            ("scroll_snap_x", lambda s: s._get_spec_int("scroll_snap_x")),
            ("scroll_snap_y", lambda s: s._get_spec_int("scroll_snap_y")),
            ("scroll_dir", lambda s: s._get_spec_int("scroll_dir")),
            ("layer_type", lambda s: s._get_spec_int("layer_type")),
            ("name", lambda s: s._get_name()),
        ])
        if include_children:
            d["children"] = self._collect_children(include_styles)
        if include_styles:
            d["styles"] = self._collect_styles()
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    def _collect_children(self, include_styles):
        \"\"\"Collect child snapshots, substituting corrupted snapshots on error.\"\"\"
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import safe_collect

        @Snapshot.fallback()
        def _snap(c):
            w = self._wrap_as_widget(c)
            return w.snapshot(
                include_children=True, include_styles=include_styles,
            ).as_dict()

        return safe_collect(self.children, _snap)

    def _collect_styles(self):
        \"\"\"Collect style snapshots, substituting corrupted entries on error.\"\"\"
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import safe_collect

        @Snapshot.fallback()
        def _snap(s):
            return s.snapshot().as_dict()

        return safe_collect(self.obj_styles, _snap)

    def _get_group_addr(self):
        \"\"\"Get group address from spec_attr, or None.\"\"\"
        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        grp = spec.safe_field("group")
        if grp is None:
            return None
        addr = int(grp)
        return hex(addr) if addr else None

    def _get_spec_int(self, field_name):
        \"\"\"Get an int field from spec_attr, or None if unavailable.\"\"\"
        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        val = spec.safe_field(field_name)
        if val is None:
            return None
        return int(val)

    def _get_scroll(self):
        \"\"\"Get scroll offset from spec_attr, or None.\"\"\"
        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        scroll = spec.safe_field("scroll")
        if scroll is None:
            return None
        return {"x": int(scroll["x"]), "y": int(scroll["y"])}

    def _get_name(self):
        \"\"\"Get object name from spec_attr, or None.\"\"\"
        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        name = spec.safe_field("name")
        if name is None or not int(name):
            return None
        return name.string()

    @staticmethod
    def _wrap_as_widget(obj):
        \"\"\"Try to wrap an LVObject as its specific widget type.\"\"\"
        try:
            from lvglgdb.lvgl.widgets import wrap_widget
            return wrap_widget(obj) or obj
        except ImportError:
            return obj


def dump_obj_info(obj: LVObject):
    from lvglgdb.lvgl.formatter import print_info
    print_info(obj.snapshot())


def dump_obj_styles(obj: ValueInput):
    \"\"\"Print all styles of an object.\"\"\"
    from lvglgdb.lvgl.formatter import print_table

    lv_obj = LVObject(obj)
    d = lv_obj.snapshot(include_styles=True)
    styles = d.get("styles")
    if not styles:
        print("No styles applied.")
        return

    def _style_row(_i, entry):
        value_str = entry["value_str"]
        color_rgb = entry.get("color_rgb")
        if color_rgb:
            r, g, b = color_rgb["r"], color_rgb["g"], color_rgb["b"]
            value_str = f"{value_str} \\033[48;2;{r};{g};{b}m  \\033[0m"
        return [entry["prop_name"], value_str]

    for s in styles:
        if "error" in s:
            print(f"(corrupted) {s.get('addr', '?')} — {s['error']}")
            continue
        print(f"[{s['index']}] {s['selector_str']}  {s['flags_str']}")
        print_table(
            s.get("properties", []), ["prop", "value"], _style_row,
            "Empty style.", align="l", numbered=False,
        )
"""

FILES['lvglgdb/lvgl/core/lv_obj_class.py'] = """import gdb

from lvglgdb.value import Value, ValueInput
from ..misc.lv_utils import format_coord


class LVObjClass(Value):
    \"\"\"LVGL object class wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: (
                f"ObjClass: {' -> '.join(d.get('class_chain', [d['name']]))}"
            )),
            ("name", "name"),
            ("base", "base_class"),
            ("size", lambda d: (
                f"{d['instance_size']} editable={d['editable']}"
                f" group_def={d['group_def']}"
            )),
            ("editable", "editable"),
            ("group_def", "group_def"),
            ("default_size", lambda d: (
                f"({d['width_def_str']}, {d['height_def_str']})"
                f" theme_inheritable={d['theme_inheritable']}"
            )),
            ("theme_in", "theme_inheritable"),
            ("_skip_if", "constructor_cb", "-", "constructor_cb"),
            ("_skip_if", "destructor_cb", "-", "destructor_cb"),
            ("_skip_if", "event_cb", "-", "event_cb"),
        ],
        "table": [
            ("size", "instance_size"),
            ("default_size", lambda d: (
                f"({d['width_def_str']}, {d['height_def_str']})"
            )),
        ],
        "empty_msg": "No object classes found.",
    }

    def __init__(self, cls: ValueInput):
        super().__init__(Value.normalize(cls, "lv_obj_class_t"))

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    @property
    def base_class(self):
        base = self.super_value("base_class")
        return LVObjClass(base) if int(base) else None

    @property
    def width_def(self) -> int:
        return int(self.super_value("width_def"))

    @property
    def height_def(self) -> int:
        return int(self.super_value("height_def"))

    @property
    def instance_size(self) -> int:
        return int(self.super_value("instance_size"))

    @property
    def editable(self) -> int:
        return int(self.super_value("editable"))

    @property
    def group_def(self) -> int:
        return int(self.super_value("group_def"))

    @property
    def theme_inheritable(self) -> bool:
        return bool(int(self.super_value("theme_inheritable")))

    @property
    def constructor_cb(self) -> Value:
        return self.super_value("constructor_cb")

    @property
    def destructor_cb(self) -> Value:
        return self.super_value("destructor_cb")

    @property
    def event_cb(self) -> Value:
        return self.super_value("event_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def __iter__(self):
        cls = self
        while cls:
            yield cls
            cls = cls.base_class

    @staticmethod
    def collect_all():
        \"\"\"Collect all lv_obj_class_t globals from the symbol table.\"\"\"
        import re

        # Search by symbol name suffix; filter by type to exclude non-class matches
        output = gdb.execute("info variables _class$", to_string=True)
        classes = []
        for line in output.splitlines():
            if "lv_obj_class_t" not in line:
                continue
            m = re.search(r"\\b(lv_\\w+_class)\\s*;", line)
            if m:
                try:
                    cls = LVObjClass(m.group(1))
                    classes.append(cls)
                except gdb.error:
                    pass
        return classes

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        base = self.base_class
        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "base_class": base.name if base else "-",
            "instance_size": self.instance_size,
            "editable": self.editable,
            "group_def": self.group_def,
            "width_def": self.width_def,
            "height_def": self.height_def,
            "width_def_str": format_coord(self.width_def),
            "height_def_str": format_coord(self.height_def),
            "theme_inheritable": self.theme_inheritable,
            "constructor_cb": fmt_cb(self.constructor_cb),
            "destructor_cb": fmt_cb(self.destructor_cb),
            "event_cb": fmt_cb(self.event_cb),
            "class_chain": [c.name for c in self.__iter__()],
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(classes):
        return [cls.snapshot() for cls in classes]
"""

FILES['lvglgdb/lvgl/core/lv_obj_flag_consts.py'] = """\"\"\"
Auto-generated object flag constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

OBJ_FLAG_NAMES = {
    1: "HIDDEN",
    2: "CLICKABLE",
    4: "CLICK_FOCUSABLE",
    8: "CHECKABLE",
    16: "SCROLLABLE",
    32: "SCROLL_ELASTIC",
    64: "SCROLL_MOMENTUM",
    128: "SCROLL_ONE",
    256: "SCROLL_CHAIN_HOR",
    512: "SCROLL_CHAIN_VER",
    1024: "SCROLL_ON_FOCUS",
    2048: "SCROLL_WITH_ARROW",
    4096: "SNAPPABLE",
    8192: "PRESS_LOCK",
    16384: "EVENT_BUBBLE",
    32768: "GESTURE_BUBBLE",
    65536: "ADV_HITTEST",
    131072: "IGNORE_LAYOUT",
    262144: "FLOATING",
    524288: "SEND_DRAW_TASK_EVENTS",
    1048576: "OVERFLOW_VISIBLE",
    2097152: "EVENT_TRICKLE",
    4194304: "STATE_TRICKLE",
    8388608: "LAYOUT_1",
    16777216: "LAYOUT_2",
    33554432: "WIDGET_1",
    67108864: "WIDGET_2",
    134217728: "USER_1",
    268435456: "USER_2",
    536870912: "USER_3",
    1073741824: "USER_4",
}

def decode_obj_flags(raw: int) -> list[str]:
    \"\"\"Decode a bitmask of lv_obj_flag_t into a list of flag names.\"\"\"
    return [name for bit, name in OBJ_FLAG_NAMES.items() if raw & bit]
"""

FILES['lvglgdb/lvgl/core/lv_obj_state_consts.py'] = """\"\"\"
Auto-generated object state constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

OBJ_STATE_NAMES = {
    0: "DEFAULT",
    1: "ALT",
    4: "CHECKED",
    8: "FOCUSED",
    16: "FOCUS_KEY",
    32: "EDITED",
    64: "HOVERED",
    128: "PRESSED",
    256: "SCROLLED",
    512: "DISABLED",
    4096: "USER_1",
    8192: "USER_2",
    16384: "USER_3",
    32768: "USER_4",
}

def decode_obj_states(raw: int) -> list[str]:
    \"\"\"Decode a bitmask of lv_state_t into a list of state names.\"\"\"
    if raw == 0:
        return ["DEFAULT"]
    return [name for bit, name in OBJ_STATE_NAMES.items() if bit and raw & bit]
"""

FILES['lvglgdb/lvgl/core/lv_observer.py'] = """from lvglgdb.value import Value, ValueInput
from ..misc.lv_ll import LVList
from .lv_observer_consts import SUBJECT_TYPE_NAMES


class LVObserver(Value):
    \"\"\"LVGL observer wrapper\"\"\"

    def __init__(self, obs: ValueInput):
        super().__init__(Value.normalize(obs, "lv_observer_t"))

    @property
    def subject(self) -> Value:
        return self.super_value("subject")

    @property
    def cb(self) -> Value:
        return self.super_value("cb")

    @property
    def target(self) -> Value:
        return self.super_value("target")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def auto_free_user_data(self) -> bool:
        return bool(int(self.super_value("auto_free_user_data")))

    @property
    def notified(self) -> bool:
        return bool(int(self.super_value("notified")))

    @property
    def for_obj(self) -> bool:
        return bool(int(self.super_value("for_obj")))

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        d = {
            "addr": hex(int(self)),
            "cb": fmt_cb(self.cb),
            "target": str(self.target),
            "for_obj": self.for_obj,
            "user_data": str(self.user_data),
            "subject_addr": ptr_or_none(self.subject),
            "target_addr": ptr_or_none(self.target),
        }
        return Snapshot(d, source=self)


class LVSubject(Value):
    \"\"\"LVGL subject wrapper\"\"\"

    _OBSERVER_FIELDS = [
        ("_title", lambda d: (
            f"  Observer: cb={d['cb']} target={d['target']}"
            f" for_obj={d['for_obj']}"
        )),
    ]

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: (
                f"Subject: type={d['type_name']}"
                f" subscribers={len(d.get('observers', []))}"
            )),
            ("_children", "observers", _OBSERVER_FIELDS),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, subject: ValueInput):
        super().__init__(Value.normalize(subject, "lv_subject_t"))

    @property
    def type(self) -> int:
        return int(self.super_value("type"))

    @property
    def type_name(self) -> str:
        return SUBJECT_TYPE_NAMES.get(self.type, f"UNKNOWN({self.type})")

    @property
    def size(self) -> int:
        return int(self.super_value("size"))

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def __iter__(self):
        for obs in LVList(self.subs_ll, "lv_observer_t"):
            yield LVObserver(obs)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        observers = []
        observer_addrs = []
        for obs in self.__iter__():
            observers.append(obs.snapshot().as_dict())
            observer_addrs.append(hex(int(obs)))

        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "size": self.size,
            "observers": observers,
            "observer_addrs": observer_addrs,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
"""

FILES['lvglgdb/lvgl/core/lv_observer_consts.py'] = """\"\"\"
Auto-generated observer constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

SUBJECT_TYPE_NAMES = {
    0: "INVALID",
    1: "NONE",
    2: "INT",
    3: "FLOAT",
    4: "POINTER",
    5: "COLOR",
    6: "GROUP",
    7: "STRING",
}
"""

FILES['lvglgdb/lvgl/data_utils.py'] = """import functools
from typing import Callable, Optional

import gdb


def fmt_cb(cb) -> str:
    \"\"\"Format callback pointer as resolved symbol string or '-' for NULL.
    Strips null bytes that may appear in some GDB output.\"\"\"
    if not cb.is_ok:
        return str(cb)
    addr = int(cb)
    if not addr:
        return "-"
    return cb.format_string(symbols=True, address=True).replace("\\x00", "")


def ptr_or_none(val) -> Optional[str]:
    \"\"\"Convert pointer to hex string or None if NULL/missing.\"\"\"
    if val is None or not val.is_ok:
        return None
    addr = int(val)
    return hex(addr) if addr else None


def safe_collect(
    items_or_label,
    transform: Callable = None,
):
    \"\"\"Unified safe collection with two usage modes.

    Iteration mode — collect items, skipping failures:
        safe_collect(items, transform)

    Decorator mode — wrap an entire collector function:
        @safe_collect("subsystem name")
        def collect_xxx(): ...

    In decorator mode, exceptions cause a gdb warning and return [].
    In iteration mode, a failing item is warned about and left out.
    \"\"\"
    if isinstance(items_or_label, str):
        label = items_or_label

        def decorator(fn):
            @functools.wraps(fn)
            def wrapper(*args, **kwargs):
                try:
                    return fn(*args, **kwargs)
                except Exception as e:
                    import traceback
                    gdb.write(f"Warning: failed to collect {label}: {e}\\n")
                    traceback.print_exc()
                    return []
            return wrapper
        return decorator

    result = []
    try:
        for item in items_or_label:
            try:
                result.append(transform(item))
            except Exception as e:
                # Dropping an object drops everything below it too, so say so
                # rather than letting a whole subtree vanish from a dump.
                gdb.write(f"Warning: skipped an item: {type(e).__name__}: {e}\\n")
                continue
    except Exception as e:
        gdb.write(f"Warning: stopped collecting early: {type(e).__name__}: {e}\\n")
    return result
"""

FILES['lvglgdb/lvgl/display/__init__.py'] = """from .lv_display import LVDisplay

__all__ = [
    "LVDisplay",
]
"""

FILES['lvglgdb/lvgl/display/lv_display.py'] = """from ..core.lv_obj import LVObject
from ..draw.lv_draw_buf import LVDrawBuf
from lvglgdb.value import Value, ValueInput


class LVDisplay(Value):
    \"\"\"LVGL display\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: f"Display @{d['addr']}"),
            ("hor_res", "hor_res"),
            ("ver_res", "ver_res"),
            ("screen_count", "screen_count"),
        ],
        "table": [],
        "empty_msg": "No displays.",
    }

    _LAYER_NAMES = ("bottom_layer", "act_scr", "top_layer", "sys_layer")

    def __init__(self, disp: ValueInput):
        super().__init__(Value.normalize(disp, "lv_display_t"))

    @property
    def hor_res(self) -> int:
        \"\"\"Get horizontal resolution in pixels\"\"\"
        return int(self.super_value("hor_res"))

    @property
    def ver_res(self) -> int:
        \"\"\"Get vertical resolution in pixels\"\"\"
        return int(self.super_value("ver_res"))

    @property
    def screen_cnt(self) -> int:
        \"\"\"Return screen count, 0 if corrupted.\"\"\"
        cnt = self.super_value("screen_cnt")
        if not cnt.is_ok:
            return 0
        return int(cnt)

    @property
    def screens(self):
        for i in range(self.screen_cnt):
            yield LVObject(self.super_value("screens")[i].read_value())

    @property
    def layer_addrs(self) -> dict:
        \"\"\"Map screen address -> layer name for known layer pointers.\"\"\"
        result = {}
        for name in self._LAYER_NAMES:
            ptr = self.super_value(name)
            if ptr.is_ok and int(ptr):
                result[int(ptr)] = name
        return result

    # Buffer-related properties
    @property
    def buf_1(self):
        \"\"\"Get first draw buffer (may be None)\"\"\"
        buf_ptr = self.super_value("buf_1")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_2(self):
        \"\"\"Get second draw buffer (may be None)\"\"\"
        buf_ptr = self.super_value("buf_2")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_act(self):
        \"\"\"Get currently active draw buffer (may be None)\"\"\"
        buf_ptr = self.super_value("buf_act")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "hor_res": self.hor_res,
            "ver_res": self.ver_res,
            "screen_count": self.screen_cnt,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
"""

FILES['lvglgdb/lvgl/draw/__init__.py'] = """from .lv_draw_buf import LVDrawBuf
from .lv_draw_task import LVDrawTask
from .lv_draw_unit import LVDrawUnit
from .lv_draw_consts import (
    DRAW_TASK_TYPE_NAMES,
    DRAW_TASK_STATE_NAMES,
    DRAW_UNIT_TYPE_NAMES,
)

__all__ = [
    "LVDrawBuf",
    "LVDrawTask",
    "DRAW_TASK_TYPE_NAMES",
    "DRAW_TASK_STATE_NAMES",
    "DRAW_UNIT_TYPE_NAMES",
    "LVDrawUnit",
]
"""

FILES['lvglgdb/lvgl/draw/image_codec.py'] = """\"\"\"Pixel conversion and PNG/BMP encoding with the standard library only.

GDB embeds whatever Python it was built against, and on many setups nothing
can be installed into it, so the plugin cannot rely on numpy or Pillow. The
conversions below use bytes slicing, `bytes.translate` and big-integer bit
operations, which all run in C, so even a large framebuffer converts in a
fraction of a second.
\"\"\"

import struct
import zlib
from typing import NamedTuple


class RawImage(NamedTuple):
    \"\"\"Packed pixels, row after row, top row first.

    `mode` is "RGB" (3 bytes per pixel) or "RGBA" (4 bytes per pixel).
    \"\"\"

    width: int
    height: int
    mode: str
    data: bytes


_BYTES_PER_PIXEL = {"RGB": 3, "RGBA": 4}

# RGB565 split into its two bytes (little endian: lo, hi):
#   hi = RRRRRGGG   lo = GGGBBBBB
_R_FROM_HI = bytes((v & 0xF8) for v in range(256))
_G_FROM_HI = bytes(((v & 0x07) << 5) for v in range(256))
_G_FROM_LO = bytes(((v & 0xE0) >> 3) for v in range(256))
_B_FROM_LO = bytes(((v & 0x1F) << 3) for v in range(256))


def _or_bytes(a: bytes, b: bytes) -> bytes:
    \"\"\"Bitwise OR of two equally long byte strings.\"\"\"
    n = len(a)
    return (int.from_bytes(a, "big") | int.from_bytes(b, "big")).to_bytes(n, "big")


def _interleave(channels, count: int) -> bytes:
    out = bytearray(count * len(channels))
    step = len(channels)
    for i, ch in enumerate(channels):
        out[i::step] = ch
    return bytes(out)


def convert(pixel_data: bytes, width: int, height: int, fmt: str) -> RawImage:
    \"\"\"Convert packed LVGL pixels to RGB or RGBA.

    `fmt` is the LVGL color format name without the prefix: RGB565, RGB888,
    ARGB8888 or XRGB8888. LVGL stores colors little endian, so RGB888 is laid
    out B, G, R in memory and the 32-bit formats B, G, R, A.
    \"\"\"
    count = width * height
    if fmt == "RGB565":
        data = pixel_data[: count * 2]
        lo, hi = data[0::2], data[1::2]
        r = hi.translate(_R_FROM_HI)
        g = _or_bytes(hi.translate(_G_FROM_HI), lo.translate(_G_FROM_LO))
        b = lo.translate(_B_FROM_LO)
        return RawImage(width, height, "RGB", _interleave((r, g, b), count))

    if fmt == "RGB888":
        data = pixel_data[: count * 3]
        return RawImage(width, height, "RGB",
                        _interleave((data[2::3], data[1::3], data[0::3]), count))

    if fmt == "ARGB8888":
        data = pixel_data[: count * 4]
        return RawImage(width, height, "RGBA",
                        _interleave((data[2::4], data[1::4], data[0::4], data[3::4]),
                                    count))

    if fmt == "XRGB8888":
        data = pixel_data[: count * 4]
        return RawImage(width, height, "RGB",
                        _interleave((data[2::4], data[1::4], data[0::4]), count))

    raise ValueError(f"Unsupported color format: {fmt}")


def _png_chunk(tag: bytes, payload: bytes) -> bytes:
    return (struct.pack(">I", len(payload)) + tag + payload
            + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))


def encode_png(img: RawImage) -> bytes:
    \"\"\"Encode as an 8-bit truecolor PNG (with alpha for RGBA).\"\"\"
    bpp = _BYTES_PER_PIXEL[img.mode]
    row = img.width * bpp
    # Every scanline starts with its filter type; 0 means no filter.
    scanlines = b"".join(
        b"\\x00" + img.data[y * row:(y + 1) * row] for y in range(img.height)
    )
    color_type = 6 if img.mode == "RGBA" else 2
    ihdr = struct.pack(">IIBBBBB", img.width, img.height, 8, color_type, 0, 0, 0)
    return (b"\\x89PNG\\r\\n\\x1a\\n"
            + _png_chunk(b"IHDR", ihdr)
            + _png_chunk(b"IDAT", zlib.compress(scanlines, 6))
            + _png_chunk(b"IEND", b""))


def encode_bmp(img: RawImage) -> bytes:
    \"\"\"Encode as an uncompressed 96 DPI BMP: 24-bit for RGB, 32-bit for RGBA.

    Like Pillow, the 32-bit variant has no alpha mask, so most viewers show it
    opaque; use PNG to keep the alpha channel.
    \"\"\"
    bpp = _BYTES_PER_PIXEL[img.mode]
    count = img.width * img.height
    data = img.data
    # BMP stores B, G, R(, A).
    if bpp == 3:
        swapped = _interleave((data[2::3], data[1::3], data[0::3]), count)
    else:
        swapped = _interleave((data[2::4], data[1::4], data[0::4], data[3::4]), count)

    row = img.width * bpp
    padded_row = (row + 3) & ~3
    pad = b"\\x00" * (padded_row - row)
    # Rows go bottom-up.
    pixels = b"".join(
        swapped[y * row:(y + 1) * row] + pad for y in range(img.height - 1, -1, -1)
    )

    header_size = 14 + 40
    file_header = struct.pack("<2sIHHI", b"BM", header_size + len(pixels), 0, 0,
                              header_size)
    info_header = struct.pack("<IiiHHIIiiII", 40, img.width, img.height, 1,
                              bpp * 8, 0, len(pixels), 3780, 3780, 0, 0)
    return file_header + info_header + pixels


ENCODERS = {"PNG": encode_png, "BMP": encode_bmp}
"""

FILES['lvglgdb/lvgl/draw/lv_draw.py'] = """"""

FILES['lvglgdb/lvgl/draw/lv_draw_buf.py'] = """from pathlib import Path
from typing import Optional
import gdb

from lvglgdb.value import Value, ValueInput
from .image_codec import ENCODERS, RawImage, convert, encode_png


class LVDrawBuf(Value):
    \"\"\"LVGL draw buffer\"\"\"

    _color_formats = {
        "RGB565": None,
        "RGB888": None,
        "ARGB8888": None,
        "XRGB8888": None,
    }

    _bpp = {
        "RGB565": 16,
        "RGB888": 24,
        "ARGB8888": 32,
        "XRGB8888": 32,
    }

    def __init__(self, draw_buf: ValueInput):
        super().__init__(Value.normalize(draw_buf, "lv_draw_buf_t"))
        self._init_color_formats()

    def _init_color_formats(self):
        \"\"\"init color formats from enum\"\"\"
        for fmt in self._color_formats.keys():
            if self._color_formats[fmt] is None:
                try:
                    self._color_formats[fmt] = int(
                        gdb.parse_and_eval(f"LV_COLOR_FORMAT_{fmt}")
                    )
                except gdb.error:
                    print(f"Warning: Failed to get LV_COLOR_FORMAT_{fmt}")
                    self._color_formats[fmt] = -1

    def color_format_info(self) -> dict:
        \"\"\"get color format info\"\"\"
        header = self.super_value("header")
        cf = int(header["cf"])

        fmt_name = "UNKNOWN"
        for name, val in self._color_formats.items():
            if val == cf:
                fmt_name = name
                break

        return {"value": cf, "name": fmt_name, "bpp": self._bpp.get(fmt_name, 0)}

    @property
    def header(self) -> dict:
        \"\"\"Get the image header information as a dictionary\"\"\"
        header = self.super_value("header")
        return {
            "w": header["w"],
            "h": header["h"],
            "stride": header["stride"],
            "cf": header["cf"],  # Color format
        }

    @property
    def data_size(self) -> int:
        \"\"\"Get the total buffer size in bytes\"\"\"
        return self.super_value("data_size")

    @property
    def data(self) -> bytes:
        \"\"\"Get the buffer data ptr\"\"\"
        return self.super_value("data")

    def _read_image(self, strict: bool = False) -> Optional[RawImage]:
        \"\"\"Read buffer data and convert it to RGB or RGBA pixels.

        Args:
            strict: If True, raise on any size mismatch instead of ignoring.

        Returns:
            RawImage or None on failure.
        \"\"\"
        header = self.super_value("header")
        stride = int(header["stride"])
        height = int(header["h"])
        cf_info = self.color_format_info()
        data_ptr = self.super_value("data")
        data_size = int(self.super_value("data_size"))
        # The width is in the header. Deriving it from the stride instead turns
        # any row padding into extra pixels, which shears the whole image.
        width = int(header["w"])
        bpp = cf_info["bpp"]
        row_bytes = (width * bpp + 7) // 8 if bpp else 0
        if not stride:
            stride = row_bytes
        expected_data_size = stride * height

        if not data_ptr or width <= 0 or height <= 0:
            if strict:
                raise ValueError(f"Invalid buffer: ptr={data_ptr}, {width}x{height}")
            return None
        if data_size < expected_data_size:
            if strict:
                raise ValueError(
                    f"Data too small: expected at least {expected_data_size},"
                    f" got {data_size}"
                )
            return None
        if data_size > expected_data_size and strict:
            gdb.write(
                f"Warning: data_size {data_size} exceeds expected"
                f" {expected_data_size}, extra bytes will be ignored\\n"
            )

        pixel_data = (
            gdb.selected_inferior()
            .read_memory(int(data_ptr), expected_data_size)
            .tobytes()
        )
        if stride > row_bytes:
            # _convert_to_image expects rows packed back to back, so drop the
            # padding at the end of each row.
            pixel_data = b"".join(
                pixel_data[y * stride:y * stride + row_bytes]
                for y in range(height)
            )
        return self._convert_to_image(pixel_data, width, height, cf_info["name"])

    def data_dump(self, filename: str, format: str = None) -> bool:
        \"\"\"Dump the buffer data to an image file.

        Args:
            filename: Output file path
            format: Image format (None for auto-detection from filename)

        Returns:
            bool: True if successful, False otherwise
        \"\"\"
        try:
            if not filename:
                raise ValueError("Output filename cannot be empty")

            img = self._read_image(strict=True)
            if img is None:
                return False

            output_format = (
                format.upper() if format else Path(filename).suffix[1:].upper() or "BMP"
            )
            encoder = ENCODERS.get(output_format)
            if encoder is None:
                raise ValueError(
                    f"Unsupported image format {output_format}, use BMP or PNG"
                )
            with open(filename, "wb") as f:
                f.write(encoder(img))

            cf_info = self.color_format_info()
            print(
                f"Successfully saved {cf_info['name']} buffer as {output_format} to {filename}"
            )
            return True

        except gdb.MemoryError:
            print("Error: Failed to access memory")
            return False
        except ValueError as e:
            print(f"Validation error: {str(e)}")
            return False
        except Exception as e:
            print(f"Unexpected error: {str(e)}")
            return False

    def to_png_bytes(self) -> Optional[bytes]:
        \"\"\"Convert buffer to PNG bytes in memory. Returns None on failure.\"\"\"
        try:
            img = self._read_image(strict=False)
            if img is None:
                return None
            return encode_png(img)
        except Exception:
            return None

    def _convert_to_image(
        self, pixel_data: bytes, width: int, height: int, color_format: str
    ) -> Optional[RawImage]:
        \"\"\"
        Convert raw pixel data to RGB or RGBA based on color format.

        Args:
            pixel_data: Raw pixel bytes, rows packed back to back
            width: Image width
            height: Image height
            color_format: LVGL color format name, e.g. "RGB565"

        Returns:
            RawImage or None if conversion fails
        \"\"\"
        try:
            return convert(pixel_data, width, height, color_format)
        except Exception as e:
            print(f"Conversion error: {str(e)}")
            return None
"""

FILES['lvglgdb/lvgl/draw/lv_draw_consts.py'] = """\"\"\"
Auto-generated draw constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

DRAW_TASK_TYPE_NAMES = {
    0: "NONE",
    1: "FILL",
    2: "BORDER",
    3: "BOX_SHADOW",
    4: "LETTER",
    5: "LABEL",
    6: "IMAGE",
    7: "LAYER",
    8: "LINE",
    9: "ARC",
    10: "TRIANGLE",
    11: "MASK_RECTANGLE",
    12: "MASK_BITMAP",
    13: "BLUR",
    14: "VECTOR",
    15: "3D",
}

DRAW_TASK_STATE_NAMES = {
    0: "BLOCKED",
    1: "WAITING",
    2: "QUEUED",
    3: "IN_PROGRESS",
    4: "FINISHED",
    5: "FAILED",
}

DRAW_UNIT_TYPE_NAMES = {
    "DAVE2D": "lv_draw_dave2d_unit_t",
    "DMA2D": "lv_draw_dma2d_unit_t",
    "ESP_PPA": "lv_draw_ppa_unit_t",
    "G2D": "lv_draw_g2d_unit_t",
    "NANOVG": "lv_draw_nanovg_unit_t",
    "NEMA_GFX": "lv_draw_nema_gfx_unit_t",
    "NXP_PXP": "lv_draw_pxp_unit_t",
    "OPENGLES": "lv_draw_opengles_unit_t",
    "SDL": "lv_draw_sdl_unit_t",
    "SW": "lv_draw_sw_unit_t",
    "SW_ARM2D": "lv_draw_sw_unit_t",
    "SW_CUSTOM": "lv_draw_sw_unit_t",
    "SW_HELIUM": "lv_draw_sw_unit_t",
    "SW_NEON": "lv_draw_sw_unit_t",
    "SW_RISCV_V": "lv_draw_sw_unit_t",
    "SW_SVE2": "lv_draw_sw_unit_t",
    "SiFli_EPIC": "lv_draw_sifli_epic_unit_t",
    "VG_LITE": "lv_draw_vg_lite_unit_t",
}
"""

FILES['lvglgdb/lvgl/draw/lv_draw_task.py'] = """from lvglgdb.value import Value, ValueInput
from .lv_draw_consts import DRAW_TASK_TYPE_NAMES, DRAW_TASK_STATE_NAMES


class LVDrawTask(Value):
    \"\"\"LVGL draw task wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("type", "type_name"),
            ("state", "state_name"),
            ("area", lambda d: (
                f"({d['area']['x1']}, {d['area']['y1']}, "
                f"{d['area']['x2']}, {d['area']['y2']})"
            )),
            ("opa", "opa"),
            ("unit_id", "preferred_draw_unit_id"),
        ],
        "table": [],
        "empty_msg": "No draw tasks.",
    }

    def __init__(self, task: ValueInput):
        super().__init__(Value.normalize(task, "lv_draw_task_t"))

    @property
    def type(self) -> int:
        return int(self.super_value("type"))

    @property
    def type_name(self) -> str:
        return DRAW_TASK_TYPE_NAMES.get(self.type, f"UNKNOWN({self.type})")

    @property
    def state(self) -> int:
        return int(self.super_value("state"))

    @property
    def state_name(self) -> str:
        return DRAW_TASK_STATE_NAMES.get(self.state, f"UNKNOWN({self.state})")

    @property
    def area(self) -> tuple:
        a = self.super_value("area")
        return (int(a.x1), int(a.y1), int(a.x2), int(a.y2))

    @property
    def opa(self) -> int:
        return int(self.super_value("opa"))

    @property
    def next(self):
        n = self.super_value("next")
        return LVDrawTask(n) if int(n) else None

    def __iter__(self):
        node = self
        while node:
            yield node
            node = node.next

    @property
    def preferred_draw_unit_id(self) -> int:
        return int(self.super_value("preferred_draw_unit_id"))

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        x1, y1, x2, y2 = self.area
        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "state": self.state,
            "state_name": self.state_name,
            "area": {"x1": x1, "y1": y1, "x2": x2, "y2": y2},
            "opa": self.opa,
            "preferred_draw_unit_id": self.preferred_draw_unit_id,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(tasks):
        return [t.snapshot() for t in tasks]
"""

FILES['lvglgdb/lvgl/draw/lv_draw_unit.py'] = """from lvglgdb.value import Value, ValueInput


class LVDrawUnit(Value):
    \"\"\"LVGL draw unit wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("name", "name"),
            ("idx", "idx"),
        ],
        "table": [],
        "empty_msg": "No draw units.",
    }

    def __init__(self, unit: ValueInput):
        super().__init__(Value.normalize(unit, "lv_draw_unit_t"))

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    @property
    def idx(self) -> int:
        return self.safe_field("idx", -1, int)

    @property
    def next(self):
        n = self.super_value("next")
        return LVDrawUnit(n) if int(n) else None

    def __iter__(self):
        node = self
        while node:
            yield node
            node = node.next

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "idx": self.idx,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(units):
        return [unit.snapshot() for unit in units]
"""

FILES['lvglgdb/lvgl/formatter.py'] = """\"\"\"Generic formatting helpers for snapshot data.

This module provides data-driven formatters that work with any snapshot dict.
No module-specific logic lives here — all customization is done via field specs
and format callbacks passed by callers.
\"\"\"
from typing import Callable, List, Optional, Sequence, Tuple, Union

from .text_table import TextTable


# ---------------------------------------------------------------------------
# Type aliases for field specs
# ---------------------------------------------------------------------------

# A field spec is a tuple describing one line of print_info output:
#   (label, key_or_fmt)
#
# - label: the left-hand label string (e.g. "var")
# - key_or_fmt: either a dict key string, or a callable(d) -> str
#
# Special forms:
#   ("_title", callable(d) -> str)   — title line (no indent)
#   ("_children", key, child_fields) — nested list of dicts
#   ("_skip_if", key, sentinel, (label, key_or_fmt))
#       — only print if d[key] != sentinel

FieldSpec = Union[
    Tuple[str, Union[str, Callable]],
    Tuple[str, str, Callable],
    Tuple[str, str, str, Tuple],
    Tuple[str, str, list],
]


def print_info(d, fields: Sequence = None, indent: int = 0) -> None:
    \"\"\"Print a snapshot dict using a declarative field spec list.

    Fields resolution order:
        1. Explicit *fields* parameter (highest priority)
        2. d._display_spec["info"] (Snapshot self-describing)
        3. None → default rendering (iterate dict keys as "key = value")
    \"\"\"
    if fields is None:
        spec = getattr(d, "_display_spec", None)
        if spec:
            fields = spec["info"]

    # Default rendering: iterate dict keys
    if fields is None:
        prefix = "  " * indent
        items = d.items() if hasattr(d, "items") else vars(d).items()
        for key, value in items:
            print(f"{prefix}  {key} = {value}")
        return

    prefix = "  " * indent
    for spec in fields:
        # Bare string shorthand: "key" → ("key", "key")
        if isinstance(spec, str):
            spec = (spec, spec)

        tag = spec[0]

        if tag == "_title":
            fmt_fn = spec[1]
            print(f"{prefix}{fmt_fn(d)}")

        elif tag == "_children":
            key, child_fields = spec[1], spec[2]
            children = d.get(key, [])
            for child in children:
                print_info(child, fields=child_fields, indent=indent + 1)

        elif tag == "_skip_if":
            key, sentinel, inner = spec[1], spec[2], spec[3]
            # Bare string shorthand for inner: "key" → ("key", "key")
            if isinstance(inner, str):
                inner = (inner, inner)
            if d.get(key) != sentinel:
                _print_field(prefix, inner, d)

        else:
            _print_field(prefix, spec, d)


def _print_field(prefix: str, spec: tuple, d) -> None:
    \"\"\"Print a single (label, key_or_fmt) field.\"\"\"
    label, key_or_fmt = spec[0], spec[1]
    if callable(key_or_fmt):
        value = key_or_fmt(d)
    else:
        value = d.get(key_or_fmt, "")
    print(f"{prefix}  {label:14s} = {value}")


# ---------------------------------------------------------------------------
# Table helpers
# ---------------------------------------------------------------------------


def print_table(
    entries: List,
    columns: List[str],
    row_fn: Callable,
    empty_msg: str,
    align: str = "l",
    numbered: bool = True,
    col_align: dict = None,
) -> None:
    \"\"\"Generic helper for printing a table from snapshot data.

    Args:
        entries: list of dict-like objects (Snapshot or plain dict).
        columns: column header names (excluding '#' if numbered=True).
        row_fn: callable(index, entry) -> list of cell values.
        empty_msg: message to print when entries is empty.
        align: default column alignment.
        numbered: if True, prepend a '#' column with row index.
        col_align: optional per-column alignment overrides, e.g. {"src": "l"}.
    \"\"\"
    table = TextTable((["#"] + columns) if numbered else columns,
                      align=align, col_align=col_align)

    for i, d in enumerate(entries):
        row = row_fn(i, d)
        if numbered:
            row = [i] + row
        table.add_row(row)

    if not table.rows:
        print(empty_msg)
    else:
        print(table)

def resolve_table_columns(spec: dict) -> tuple:
    \"\"\"Resolve Display_Spec into (column_headers, auto_row_fn).

    Merges the ``info`` field list with ``table`` overrides to produce a flat
    list of column headers and a row-extraction function with ``(i, d)``
    signature suitable for :func:`print_table`.

    Algorithm:
        1. Walk ``spec["info"]``, skip ``_title`` / ``_children``, normalise
           bare strings, and collect ``(label, key_or_fmt, skip_info)`` triples.
        2. Walk ``spec["table"]``, build an override map (keyed by label) and
           an append list for labels not present in the base columns.
        3. Apply overrides in-place, then append new columns.
        4. Return ``(headers, auto_row_fn)``.
    \"\"\"
    # -- 1. base columns from info ----------------------------------------
    base_columns = []
    base_labels = set()
    for entry in spec["info"]:
        if isinstance(entry, str):
            entry = (entry, entry)

        tag = entry[0]
        if tag in ("_title", "_children"):
            continue

        if tag == "_skip_if":
            skip_key, sentinel, inner = entry[1], entry[2], entry[3]
            if isinstance(inner, str):
                inner = (inner, inner)
            label, key_or_fmt = inner[0], inner[1]
            base_columns.append((label, key_or_fmt, (skip_key, sentinel)))
        else:
            label, key_or_fmt = entry[0], entry[1]
            base_columns.append((label, key_or_fmt, None))

        base_labels.add(base_columns[-1][0])

    # -- 2. table overrides and appends -----------------------------------
    override_map = {}
    append_list = []
    for entry in spec.get("table", []):
        if isinstance(entry, str):
            entry = (entry, entry)

        tag = entry[0]
        if tag == "_skip_if":
            skip_key, sentinel, inner = entry[1], entry[2], entry[3]
            if isinstance(inner, str):
                inner = (inner, inner)
            label, key_or_fmt = inner[0], inner[1]
            item = (label, key_or_fmt, (skip_key, sentinel))
        else:
            label, key_or_fmt = entry[0], entry[1]
            item = (label, key_or_fmt, None)

        if label in base_labels:
            override_map[label] = item
        else:
            append_list.append(item)

    # -- 3. merge ---------------------------------------------------------
    resolved = []
    for label, key_or_fmt, skip in base_columns:
        if label in override_map:
            resolved.append(override_map[label])
        else:
            resolved.append((label, key_or_fmt, skip))
    resolved.extend(append_list)

    # -- 4. build outputs -------------------------------------------------
    headers = [label for label, _, _ in resolved]

    def auto_row_fn(i, d):
        row = []
        for _label, key_or_fmt, skip in resolved:
            if skip is not None:
                sk, sentinel = skip
                if d.get(sk) == sentinel:
                    row.append("")
                    continue
            if callable(key_or_fmt):
                row.append(key_or_fmt(d))
            else:
                row.append(d.get(key_or_fmt, ""))
        return row

    return (headers, auto_row_fn)

def print_spec_table(
    entries: list,
    spec: dict = None,
    align: str = "l",
    numbered: bool = True,
    col_align: dict = None,
    extra_columns: list = None,
    extra_row_fn: Callable = None,
) -> None:
    \"\"\"Render a table from a Display_Spec.

    Resolves columns from *spec* via :func:`resolve_table_columns`, optionally
    prepends *extra_columns* (used by DumpCache for dynamic fields), then
    delegates to :func:`print_table`.

    If *spec* is not provided, it is read from the first entry's
    ``_display_spec`` attribute.
    \"\"\"
    if spec is None and entries:
        spec = getattr(entries[0], "_display_spec", None)
    if spec is None:
        print("")
        return
    columns, auto_row_fn = resolve_table_columns(spec)

    if extra_columns and extra_row_fn:
        full_columns = extra_columns + columns

        def combined_row_fn(i, d):
            return extra_row_fn(d) + auto_row_fn(i, d)

        row_fn = combined_row_fn
    else:
        full_columns = columns
        row_fn = auto_row_fn

    print_table(
        entries,
        full_columns,
        row_fn,
        spec["empty_msg"],
        align=align,
        numbered=numbered,
        col_align=col_align,
    )







"""

FILES['lvglgdb/lvgl/misc/README.md'] = """# LVGL GDB Scripts - Misc Module Extensions

This directory contains GDB debugging scripts for LVGL's miscellaneous data structures, including red-black trees and
cache systems.

## Objects

### 1. lv_rb.py - Red-Black Tree Support

Provides debugging support for LVGL's red-black tree (`lv_rb_t`) implementation.

**Classes:**

- `LVRedBlackTree` - Iterator for red-black tree nodes

**Functions:**

- `dump_rb_info(rb, datatype)` - Dump basic red-black tree information

**Usage:**

```gdbscript
# In GDB:
python tree = LVRedBlackTree(some_rb_tree)
python for data in tree: print(data)
python dump_rb_info(some_rb_tree, datatype)
```

### 2. lv_cache.py - Cache System Support

Provides debugging support for LVGL's cache system (`lv_cache_t`).

**Classes:**

- `LVCache` - Wrapper for cache objects

**Functions:**

- `dump_cache_info(cache, datatype)` - Dump cache information

**Usage:**

```gdbscript
# In GDB:
python cache = LVCache(some_cache)
python print(f"Cache usage: {cache.get_usage_percentage()}%")
python dump_cache_info(some_cache, datatype)
```

### 3. lv_cache_entry.py - Cache Entry Support

Provides debugging support for cache entries (`lv_cache_entry_t`).

**Classes:**

- `LVCacheEntry` - Wrapper for cache entries

**Functions:**

- `dump_cache_entry_info(entry, datatype)` - Dump cache entry information

**Usage:**

```gdbscript
# In GDB:
python entry = LVCacheEntry(some_entry)
python print(f"Ref count: {entry.get_ref_count()}")
python dump_cache_entry_info(some_entry, datatype)
```

### 4. lv_cache_lru_rb.py - LRU Cache with RB Tree Support

Specialized support for LRU-based caches using red-black trees.

**Classes:**

- `LVCacheLRURB` - Specialized cache wrapper for LRU RB implementations

**Functions:**

- `dump_lru_rb_cache_info(cache)` - Dump LRU RB cache information

### 5. lv_image_cache.py and lv_image_header_cache.py - Image Cache Support

Provides debugging support for LVGL's image cache (`lv_image_cache_t`).

**Classes:**

- `LVImageCache` - Wrapper for image cache objects

**Usage:**

```gdbscript
# In GDB:
python curr_inst().image_cache().print_info()
python curr_inst().image_cache().print_entries()
```

## Integration

All these modules are automatically imported and available when using the LVGL GDB scripts. You can access them through
the main `lvgl` module:

```gdbscript
# In GDB:
python from lvglgdb import LVRedBlackTree, LVCache, dump_rb_info, dump_cache_info
```

## Type Conversion Support

All wrapper classes support multiple input types:

- `gdb.Value` objects (from GDB expressions)
- `Value` objects (from lvglgdb.value module)
- Raw pointers as integers (e.g., `0x12345678`)

This allows flexible usage patterns:

```gdbscript
# From GDB variable
entry = LVCacheEntry(my_entry)

# From raw memory address
entry = LVCacheEntry(0x7fff12345678)

# From GDB expression result
entry = LVCacheEntry(gdb.parse_and_eval("*(lv_cache_entry_t*)0x7fff12345678"))
```

## Examples

### Debugging a Red-Black Tree

```gdbscript
# Create tree iterator from variable
tree = LVRedBlackTree(some_rb_variable)

# Create from raw pointer (0x12345678)
tree = LVRedBlackTree(0x12345678)

# Iterate through all nodes
for data_ptr in tree:
    print(f"Data pointer: {data_ptr}")

# Get tree info
print(f"Tree has {len(tree)} nodes")
tree.print_info()
```

### Debugging a Cache

```gdbscript
# Create cache wrapper from variable
cache = LVCache(some_cache_variable)

# Create from raw pointer (0x12345678)
cache = LVCache(0x12345678)

# Check cache status
print(f"Cache usage: {cache.get_usage_percentage():.1f}%")
print(f"Cache enabled: {cache.is_enabled()}")

# Dump full info
cache.print_info()
```

### Debugging Cache Entries

```gdbscript
# Create entry wrapper from variable
entry = LVCacheEntry(some_entry_variable)

# Create from raw pointer (0x12345678)
entry = LVCacheEntry(0x12345678)

# Check entry status
print(f"Ref count: {entry.get_ref_count()}")
print(f"Invalid: {entry.is_invalid()}")

# Dump full info
entry.print_info()
```

## File Structure

- `lv_rb.py` - Red-black tree debugging support
- `lv_cache.py` - General cache debugging support
- `lv_cache_entry.py` - Cache entry debugging support
- `lv_cache_lru_rb.py` - LRU RB cache specialized support
- `lv_image_cache.py` - Image cache debugging support
- `lv_image_header_cache.py` - Image header cache debugging support
- `lv_cache_iter_base.py` - Cache iterator base class
- `lv_cache_iter_factory.py` - Cache iterator factory functions
- `__init__.py` - Module exports and initialization
"""

FILES['lvglgdb/lvgl/misc/__init__.py'] = """from .lv_ll import LVList
from .lv_style import (
    LVStyle,
    StyleEntry,
    style_prop_name,
    decode_selector,
    format_style_value,
    read_symbol,
)
from .lv_rb import LVRedBlackTree
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry
from .lv_cache_lru_rb import LVCacheLRURB, LVCacheLRURBIterator
from .lv_cache_iter_base import LVCacheIteratorBase
from .lv_cache_iter_factory import create_cache_iterator
from .lv_image_cache import LVImageCache
from .lv_image_header_cache import LVImageHeaderCache
from .lv_array import LVArray
from .lv_event import (
    LVEvent,
    LVEventDsc,
    LVEventList,
    event_code_name,
    EVENT_CODE_NAMES,
)
from .lv_anim import LVAnim
from .lv_timer import LVTimer
from .lv_image_decoder import LVImageDecoder
from .lv_fs import LVFsDrv
from .lv_utils import format_coord, format_enum

__all__ = [
    "LVList",
    "LVStyle",
    "StyleEntry",
    "style_prop_name",
    "decode_selector",
    "format_style_value",
    "LVRedBlackTree",
    "LVCache",
    "LVCacheEntry",
    "LVCacheLRURB",
    "LVCacheIteratorBase",
    "LVCacheLRURBIterator",
    "LVImageCache",
    "LVImageHeaderCache",
    "create_cache_iterator",
    "LVArray",
    "LVEvent",
    "LVEventDsc",
    "LVEventList",
    "event_code_name",
    "EVENT_CODE_NAMES",
    "LVAnim",
    "LVTimer",
    "LVImageDecoder",
    "LVFsDrv",
    "format_coord",
    "format_enum",
    "read_symbol",
]
"""

FILES['lvglgdb/lvgl/misc/lv_anim.py'] = """from lvglgdb.value import Value, ValueInput


class LVAnim(Value):
    \"\"\"LVGL animation wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: f"Animation @{d['addr']}"),
            "var",
            "exec_cb",
            "path_cb",
            "start_cb",
            "completed_cb",
            "deleted_cb",
            "user_data",
            ("value", lambda d: (
                f"{d['start_value']} -> {d['current_value']}"
                f" -> {d['end_value']}"
            )),
            ("duration", lambda d: (
                f"{d['duration']}ms  act_time={d['act_time']}ms"
            )),
            ("repeat", lambda d: (
                f"{'inf' if d['repeat_cnt'] == 0xFFFFFFFF else d['repeat_cnt']}"
                f"  repeat_delay={d['repeat_delay']}ms"
            )),
            ("reverse", lambda d: (
                f"dur={d['reverse_duration']}ms"
                f"  delay={d['reverse_delay']}ms"
            )),
            ("status", lambda d: (
                f"{d['status']}  early_apply={d['early_apply']}"
            )),
        ],
        "table": [
            ("value(start/cur/end)", lambda d: (
                f"{d['start_value']}/{d['current_value']}/{d['end_value']}"
            )),
            ("duration", lambda d: f"{d['duration']}ms"),
            ("act_time", lambda d: f"{d['act_time']}ms"),
            ("repeat", lambda d: (
                "inf" if d["repeat_cnt"] == 0xFFFFFFFF
                else str(d["repeat_cnt"])
            )),
        ],
        "empty_msg": "No active animations.",
    }

    def __init__(self, anim: ValueInput):
        super().__init__(Value.normalize(anim, "lv_anim_t"))

    @property
    def var(self) -> Value:
        return self.super_value("var")

    @property
    def exec_cb(self) -> Value:
        return self.super_value("exec_cb")

    @property
    def start_cb(self) -> Value:
        return self.super_value("start_cb")

    @property
    def completed_cb(self) -> Value:
        return self.super_value("completed_cb")

    @property
    def deleted_cb(self) -> Value:
        return self.super_value("deleted_cb")

    @property
    def path_cb(self) -> Value:
        return self.super_value("path_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def start_value(self) -> int:
        return int(self.super_value("start_value"))

    @property
    def current_value(self) -> int:
        return int(self.super_value("current_value"))

    @property
    def end_value(self) -> int:
        return int(self.super_value("end_value"))

    @property
    def duration(self) -> int:
        return int(self.super_value("duration"))

    @property
    def act_time(self) -> int:
        return int(self.super_value("act_time"))

    @property
    def reverse_duration(self) -> int:
        return self.safe_field("reverse_duration", 0, int)

    @property
    def reverse_delay(self) -> int:
        return self.safe_field("reverse_delay", 0, int)

    @property
    def repeat_delay(self) -> int:
        return int(self.super_value("repeat_delay"))

    @property
    def repeat_cnt(self) -> int:
        return int(self.super_value("repeat_cnt"))

    @property
    def is_paused(self) -> bool:
        return self.safe_field("is_paused", False, bool)

    @property
    def reverse_play_in_progress(self) -> bool:
        return self.safe_field("reverse_play_in_progress", False, bool)

    @property
    def early_apply(self) -> bool:
        return self.safe_field("early_apply", False, bool)

    def _status_str(self) -> str:
        \"\"\"Short status string for table display.\"\"\"
        if self.is_paused:
            return "paused"
        if self.reverse_play_in_progress:
            return "reverse"
        return "running"

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        d = {
            "addr": hex(int(self)),
            "var": str(self.var),
            "var_addr": ptr_or_none(self.var),
            "exec_cb": fmt_cb(self.exec_cb),
            "path_cb": fmt_cb(self.path_cb),
            "start_cb": fmt_cb(self.start_cb),
            "completed_cb": fmt_cb(self.completed_cb),
            "deleted_cb": fmt_cb(self.deleted_cb),
            "user_data": str(self.user_data),
            "start_value": self.start_value,
            "current_value": self.current_value,
            "end_value": self.end_value,
            "duration": self.duration,
            "act_time": self.act_time,
            "repeat_cnt": self.repeat_cnt,
            "repeat_delay": self.repeat_delay,
            "reverse_duration": self.reverse_duration,
            "reverse_delay": self.reverse_delay,
            "status": self._status_str(),
            "early_apply": self.early_apply,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(anims):
        return [anim.snapshot() for anim in anims]
"""

FILES['lvglgdb/lvgl/misc/lv_array.py'] = """from typing import Union

import gdb

from lvglgdb.value import Value, ValueInput


class LVArray(Value):
    \"\"\"LVGL dynamic array wrapper\"\"\"

    def __init__(self, arr: ValueInput, element_type: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(arr, "lv_array_t"))
        self.element_type = (
            gdb.lookup_type(element_type)
            if isinstance(element_type, str)
            else element_type
        )

    @property
    def size(self) -> int:
        return int(self.super_value("size"))

    @property
    def capacity(self) -> int:
        return int(self.super_value("capacity"))

    @property
    def element_size(self) -> int:
        return int(self.super_value("element_size"))

    def __len__(self) -> int:
        return self.size

    def _element_at(self, data: Value, index: int) -> Value:
        \"\"\"Get element at index from data pointer.\"\"\"
        addr = int(data) + index * self.element_size
        val = Value(gdb.Value(addr))
        if self.element_type:
            return val.cast(self.element_type, ptr=True).dereference()
        return val

    def __iter__(self):
        data = self.super_value("data")
        for i in range(self.size):
            yield self._element_at(data, i)

    def __getitem__(self, index):
        if isinstance(index, str):
            return super().__getitem__(index)
        if index < 0 or index >= self.size:
            raise IndexError(f"index {index} out of range [0, {self.size})")
        return self._element_at(self.super_value("data"), index)
"""

FILES['lvglgdb/lvgl/misc/lv_cache.py'] = """from typing import Union
import gdb

from lvglgdb.value import Value, ValueInput
from .lv_cache_iter_factory import create_cache_iterator


class LVCache(Value):
    \"\"\"LVGL cache wrapper - focuses on cache-level operations\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Cache Info:"),
            ("Name", "name"),
            ("Node Size", "node_size"),
            ("Max Size", "max_size"),
            ("Current Size", "current_size"),
            ("Free Size", lambda d: d["max_size"] - d["current_size"]),
            ("Enabled", "enabled"),
            ("_skip_if", "iterator_type", None, ("Iterator Type", "iterator_type")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: ValueInput, datatype: Union[gdb.Type, str]):
        super().__init__(Value.normalize(cache, "lv_cache_t"))
        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        iter_type = None
        try:
            iterator = create_cache_iterator(self)
            iter_type = iterator.__class__.__name__
        except Exception:
            pass

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "node_size": int(self.node_size),
            "max_size": int(self.max_size),
            "current_size": int(self.size),
            "enabled": bool(int(self.max_size) > 0),
            "iterator_type": iter_type,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    def is_enabled(self):
        \"\"\"Check if cache is enabled\"\"\"
        return int(self.max_size) > 0

    def get_usage_percentage(self):
        \"\"\"Get cache usage percentage\"\"\"
        if int(self.max_size) == 0:
            return 0.0
        return int(self.size) / int(self.max_size) * 100.0

    def __iter__(self):
        \"\"\"Create appropriate iterator based on cache class\"\"\"
        iterator = create_cache_iterator(self)
        if iterator is None:
            # Returning None here would make `for entry in cache` fail with
            # TypeError about NoneType, which says nothing about the cache.
            raise gdb.GdbError(f"unsupported cache type: {self.name}")
        return iterator

    def items(self):
        \"\"\"Get all cache entries as a list\"\"\"
        entries = []
        for entry in self:
            entries.append(entry)
        return entries

    def sanity_check(self, entry_checker=None):
        \"\"\"Run sanity check and print results as a table\"\"\"
        from lvglgdb.lvgl.formatter import print_table

        try:
            iterator = iter(self)
        except gdb.GdbError as unsupported:
            iterator = ()
            errors = [str(unsupported)]
        else:
            errors = iterator.sanity_check(entry_checker)

        if errors:
            rows = [{"status": "FAIL", "detail": e} for e in errors]
        else:
            rows = [{"status": "PASS", "detail": f"all {len(iterator)} entries OK"}]

        print_table(rows, ["status", "detail"],
                    lambda i, d: [d["status"], d["detail"]], "",
                    col_align={"detail": "l"})
        return errors


"""

FILES['lvglgdb/lvgl/misc/lv_cache_entry.py'] = """from typing import Union
import gdb

from lvglgdb.value import CorruptedError, Value, ValueInput


class LVCacheEntry(Value):
    \"\"\"LVGL cache entry wrapper - focuses on entry-level operations\"\"\"

    def __init__(self, entry: ValueInput, datatype: Union[gdb.Type, str]):
        super().__init__(Value.normalize(entry, "lv_cache_entry_t"))
        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

    @classmethod
    def from_data_ptr(cls, data_ptr: ValueInput, datatype: Union[gdb.Type, str]):
        \"\"\"Create LVCacheEntry from data pointer\"\"\"

        if data_ptr.type == gdb.lookup_type("void").pointer() and datatype is None:
            raise ValueError("Data pointer is void*, datatype must be provided")

        data_ptr = data_ptr.cast(datatype)

        entry_ptr = int(data_ptr) + data_ptr.type.target().sizeof
        return cls(entry_ptr, datatype)


    def get_data(self):
        \"\"\"Get entry data pointer\"\"\"
        data_ptr = Value(int(self) - self.get_node_size()).cast(self.datatype)
        return data_ptr

    def is_invalid(self):
        \"\"\"Check if entry is invalid\"\"\"
        return bool(int(self.flags) & 1)  # LV_CACHE_ENTRY_FLAG_INVALID

    def is_disabled_delete(self):
        \"\"\"Check if entry has disable delete flag\"\"\"
        return bool(int(self.flags) & 2)  # LV_CACHE_ENTRY_FLAG_DISABLE_DELETE

    def get_ref_count(self):
        \"\"\"Get reference count\"\"\"
        return int(self.ref_cnt)

    def get_node_size(self):
        \"\"\"Get node size\"\"\"
        return int(self.node_size)

    def get_flags(self):
        \"\"\"Get flags\"\"\"
        return int(self.flags)

    def __str__(self):
        \"\"\"Provide better string representation for debugging\"\"\"
        try:
            data = self.get_data()
            return f"CacheEntry(ref_cnt={self.get_ref_count()}, valid={not self.is_invalid()}, data={data.dereference()})"
        except CorruptedError:
            pass

        return super().__str__()


"""

FILES['lvglgdb/lvgl/misc/lv_cache_iter_base.py'] = """from typing import List, Optional
import gdb

from lvglgdb.value import Value


class LVCacheIteratorBase:
    \"\"\"Base class for cache iterators\"\"\"

    def __init__(self, cache):
        self.cache = cache
        self._entries: List[Value] = []
        self._current_index = 0
        self._collect_error: Optional[str] = None
        self._collect_entries()

    def __iter__(self):
        return self

    def __next__(self):
        \"\"\"Get next cache entry\"\"\"
        if self._current_index < len(self._entries):
            entry = self._entries[self._current_index]
            self._current_index += 1
            return entry
        else:
            raise StopIteration

    def __len__(self):
        return len(self._entries)

    def __str__(self):
        return f"{self.__class__.__name__}(cache={self.cache.name}, entries={len(self._entries)})"

    def _collect_entries(self):
        \"\"\"To be implemented by subclasses\"\"\"
        raise NotImplementedError("Subclasses must implement _collect_entries")

    @property
    def extra_fields(self):
        \"\"\"Extra column names provided by this iterator, override in subclasses\"\"\"
        return []

    def get_extra(self, entry):
        \"\"\"Get extra column values for an entry, override in subclasses\"\"\"
        return []

    def sanity_check(self, entry_checker=None):
        \"\"\"Run sanity check on cache entries, override in subclasses for structural checks.
        Args:
            entry_checker: optional callable(entry) -> list of error strings
        Returns:
            list of error strings, empty means all good
        \"\"\"
        self._entries.clear()
        self._current_index = 0
        self._collect_error = None
        self._collect_entries()

        errors = []
        if self._collect_error:
            errors.append(self._collect_error)
        if entry_checker:
            for entry in self._entries:
                try:
                    errs = entry_checker(entry)
                    if errs:
                        errors.extend(errs)
                except Exception as e:
                    errors.append(f"entry_checker raised: {e}")
        return errors
"""

FILES['lvglgdb/lvgl/misc/lv_cache_iter_factory.py'] = """import gdb
import re


from .lv_cache_iter_base import LVCacheIteratorBase


def create_cache_iterator(cache) -> LVCacheIteratorBase | None:
    \"\"\"Factory function to create appropriate iterator for cache class\"\"\"

    from .lv_cache_lru_rb import LVCacheLRURB

    def get_class(s):
        if s == "lv_cache_class_lru_rb_size":
            return LVCacheLRURB
        elif s == "lv_cache_class_lru_rb_count":
            return LVCacheLRURB
        return None

    try:
        if cache.clz:
            symbol = cache.clz.format_string(symbols=True, address=False)
            m = re.match("<(.*)>", symbol)
            if m:
                symbol = m.group(1)

            cache_class = get_class(symbol)
            if cache_class:
                return iter(cache_class(cache))

    except Exception:
        raise TypeError(f"Unsupported cache type {cache}")

    # An unknown or null class: None is the documented answer, and callers check
    # for it. LVCache.__iter__ turns it into an error that names the cache, so
    # `for entry in cache` cannot fail with a bare TypeError about NoneType.
    return None
"""

FILES['lvglgdb/lvgl/misc/lv_cache_lru_rb.py'] = """import gdb

from lvglgdb.value import Value, ValueInput
from .lv_cache_iter_base import LVCacheIteratorBase
from .lv_cache_entry import LVCacheEntry
from .lv_cache import LVCache
from .lv_ll import LVList


class LVCacheLRURBIterator(LVCacheIteratorBase):
    \"\"\"Iterator for LRU RB cache implementation - traverses linked list in LRU order\"\"\"

    def __init__(self, cache):
        super().__init__(cache)

    def _get_lru_params(self):
        \"\"\"Get common parameters for ll traversal\"\"\"
        lru_cache = self.cache.cast("lv_lru_rb_t_", ptr=True)
        if not lru_cache:
            return None
        rb_size = int(lru_cache.rb.size)
        ptr_size = gdb.lookup_type("void").pointer().sizeof
        rb_node_pp_t = gdb.lookup_type("lv_rb_node_t").pointer().pointer()
        void_pp_t = gdb.lookup_type("void").pointer().pointer()
        return lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t

    def _iter_ll_nodes(self, lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t):
        \"\"\"Yield (ll_addr, data, back_ptr) for each ll node.
        Skips nodes with inaccessible memory (e.g. corrupted pointers).
        \"\"\"
        for ll_node in LVList(lru_cache.ll):
            ll_addr = int(ll_node)
            # The node holds a pointer to the rb node, so the cast is to
            # lv_rb_node_t ** and the pointer has to be read through before any
            # field of it can be.
            rb_node = Value(ll_node).cast(rb_node_pp_t)
            if not rb_node:
                continue
            rb_node = rb_node.dereference()
            if not rb_node:
                continue
            data = rb_node.data
            if not data:
                continue
            data = data.read_value()
            if not data.is_ok:
                continue
            data_addr = int(data)
            if data_addr == 0:
                continue
            back_ptr = int(
                Value(data_addr + rb_size - ptr_size).cast(void_pp_t).dereference()
            )
            yield ll_addr, data, back_ptr

    def _collect_entries(self):
        \"\"\"Collect entries from LRU RB cache by traversing the linked list (MRU→LRU order)\"\"\"
        try:
            params = self._get_lru_params()
            if not params:
                return
            lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t = params

            for ll_addr, data, back_ptr in self._iter_ll_nodes(
                lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t
            ):
                try:
                    entry = LVCacheEntry.from_data_ptr(data, self.cache.datatype)
                    entry.extra = Value(back_ptr)
                    entry.ll_addr = ll_addr
                    self._entries.append(entry)
                except Exception:
                    continue

        except Exception as e:
            self._collect_error = f"_collect_entries failed: {e}"
            import traceback

            traceback.print_exc()

    @property
    def extra_fields(self):
        return ["ll"]

    def get_extra(self, entry):
        return [f"{int(entry.extra):#x}"]

    def sanity_check(self, entry_checker=None):
        \"\"\"Verify rb tree and ll linked list consistency:
        1. Node sets match between rb and ll
        2. Cross-pointers form a closed loop: ll_node→rb_node and rb_node.back_ptr→ll_node
        \"\"\"
        errors = super().sanity_check(entry_checker)

        try:
            from .lv_rb import LVRedBlackTree

            params = self._get_lru_params()
            if not params:
                errors.append("failed to cast cache to lv_lru_rb_t_")
                return errors
            lru_cache = params[0]

            ll_data_set = set()
            ll_data_list = []
            for entry in self._entries:
                data_addr = int(entry.get_data())
                ll_data_set.add(data_addr)
                ll_data_list.append(data_addr)

                back_ptr = int(entry.extra)
                ll_addr = entry.ll_addr
                if back_ptr != ll_addr:
                    errors.append(
                        f"cross-ptr mismatch: data {data_addr:#x} "
                        f"back_ptr={back_ptr:#x} != ll_node={ll_addr:#x}"
                    )

            rb_data_set = set()
            rb_data_list = []
            rb_tree = LVRedBlackTree(lru_cache.rb)
            for data in rb_tree:
                rb_data_set.add(int(data))
                rb_data_list.append(int(data))

            if len(ll_data_list) != len(ll_data_set):
                from collections import Counter

                for addr, cnt in Counter(ll_data_list).items():
                    if cnt > 1:
                        errors.append(f"duplicate in ll: {addr:#x} appears {cnt} times")

            if len(rb_data_list) != len(rb_data_set):
                from collections import Counter

                for addr, cnt in Counter(rb_data_list).items():
                    if cnt > 1:
                        errors.append(f"duplicate in rb: {addr:#x} appears {cnt} times")

            only_in_ll = ll_data_set - rb_data_set
            only_in_rb = rb_data_set - ll_data_set

            for addr in only_in_ll:
                errors.append(f"node {addr:#x} in ll but not in rb tree")
            for addr in only_in_rb:
                errors.append(f"node {addr:#x} in rb tree but not in ll")

            if not errors:
                ll_count = len(ll_data_set)
                rb_count = len(rb_data_set)
                if ll_count != rb_count:
                    errors.append(f"count mismatch: ll={ll_count}, rb={rb_count}")

        except Exception as e:
            errors.append(f"sanity_check error: {e}")

        return errors


class LVCacheLRURB(LVCache):
    \"\"\"LVGL LRU-based cache using red-black tree iterator\"\"\"

    def __init__(self, cache: ValueInput):
        datatype = cache.datatype if isinstance(cache, LVCache) else None
        super().__init__(cache, datatype)
        self.cache_base = Value(self)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        base = super().snapshot()
        d = base.as_dict()
        d["type"] = "lru_rb"
        return Snapshot(d, source=self,
                       display_spec=getattr(base, "_display_spec", None))

    def is_count_based(self):
        \"\"\"Check if this is count-based LRU cache\"\"\"
        name = self.name
        return "count" in name.lower() or "lru_rb_count" in str(self.clz).lower()

    def is_size_based(self):
        \"\"\"Check if this is size-based LRU cache\"\"\"
        name = self.name
        return "size" in name.lower() or "lru_rb_size" in str(self.clz).lower()

    def __iter__(self):
        \"\"\"Create iterator for this LRU RB cache\"\"\"
        return LVCacheLRURBIterator(self)

    def items(self):
        \"\"\"Get all cache entries as a list\"\"\"
        entries = []
        for entry in self:
            entries.append(entry)
        return entries
"""

FILES['lvglgdb/lvgl/misc/lv_event.py'] = """import gdb

from lvglgdb.value import Value, ValueInput
from .lv_event_consts import EVENT_CODE_NAMES
from .lv_array import LVArray


def event_code_name(code: int) -> str:
    \"\"\"Map event code integer to human-readable name.\"\"\"
    return EVENT_CODE_NAMES.get(code, f"UNKNOWN({code})")


class LVEvent(Value):
    \"\"\"LVGL event wrapper\"\"\"

    def __init__(self, event: ValueInput):
        super().__init__(Value.normalize(event, "lv_event_t"))

    @property
    def code(self) -> int:
        return int(self.super_value("code"))

    @property
    def code_name(self) -> str:
        return event_code_name(self.code)

    @property
    def current_target(self) -> Value:
        return self.super_value("current_target")

    @property
    def original_target(self) -> Value:
        return self.super_value("original_target")

    @property
    def param(self) -> Value:
        return self.super_value("param")

    @property
    def prev(self):
        p = self.super_value("prev")
        return LVEvent(p) if int(p) else None

    @property
    def deleted(self) -> bool:
        return bool(int(self.super_value("deleted")))

    @property
    def stop_processing(self) -> bool:
        return bool(int(self.super_value("stop_processing")))



class LVEventDsc(Value):
    \"\"\"LVGL event descriptor wrapper\"\"\"

    def __init__(self, dsc: ValueInput):
        super().__init__(Value.normalize(dsc, "lv_event_dsc_t"))

    @property
    def cb(self) -> Value:
        return self.super_value("cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def filter(self) -> int:
        return int(self.super_value("filter"))

    @property
    def filter_code(self) -> int:
        \"\"\"Event code with flag bits masked off.\"\"\"
        return self.filter & ~(0x8000 | 0x10000)

    @property
    def filter_name(self) -> str:
        return event_code_name(self.filter_code)

    @property
    def is_preprocess(self) -> bool:
        return bool(self.filter & 0x8000)

    @property
    def is_marked_deleting(self) -> bool:
        return bool(self.filter & 0x10000)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        flags = []
        if self.is_preprocess:
            flags.append("PRE")
        if self.is_marked_deleting:
            flags.append("DEL")
        d = {
            "cb": fmt_cb(self.cb),
            "filter": self.filter_code,
            "filter_name": self.filter_name,
            "is_preprocess": self.is_preprocess,
            "is_marked_deleting": self.is_marked_deleting,
            "flags_str": ",".join(flags) or "-",
            "user_data": str(self.user_data),
        }
        return Snapshot(d, source=self, display_spec=LVEventList._DISPLAY_SPEC)


class LVEventList(Value):
    \"\"\"LVGL event list wrapper (lv_event_list_t contains lv_array_t)\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("callback", "cb"),
            ("filter", "filter_name"),
            ("flags", lambda d: d.get("flags_str", "-")),
            ("user_data", "user_data"),
        ],
        "table": [],
        "empty_msg": "No event descriptors.",
    }

    def __init__(self, event_list: ValueInput):
        super().__init__(Value.normalize(event_list, "lv_event_list_t"))

    @property
    def array(self) -> LVArray:
        \"\"\"Array stores lv_event_dsc_t* pointers (not inline structs).\"\"\"
        dsc_ptr_type = gdb.lookup_type("lv_event_dsc_t").pointer()
        return LVArray(self.super_value("array"), dsc_ptr_type)

    @property
    def is_traversing(self) -> bool:
        return bool(int(self.super_value("is_traversing")))

    @property
    def has_marked_deleting(self) -> bool:
        return bool(int(self.super_value("has_marked_deleting")))

    def __iter__(self):
        \"\"\"Yield LVEventDsc for each pointer stored in the array.\"\"\"
        for dsc_ptr in self.array:
            yield LVEventDsc(dsc_ptr)

    def __len__(self) -> int:
        return len(self.array)

    @staticmethod
    def snapshots(event_dscs):
        return [dsc.snapshot() for dsc in event_dscs]
"""

FILES['lvglgdb/lvgl/misc/lv_event_consts.py'] = """\"\"\"
Auto-generated event constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

EVENT_CODE_NAMES = {
    0: "ALL",
    1: "PRESSED",
    2: "PRESSING",
    3: "PRESS_LOST",
    4: "SHORT_CLICKED",
    5: "SINGLE_CLICKED",
    6: "DOUBLE_CLICKED",
    7: "TRIPLE_CLICKED",
    8: "LONG_PRESSED",
    9: "LONG_PRESSED_REPEAT",
    10: "CLICKED",
    11: "RELEASED",
    12: "SCROLL_BEGIN",
    13: "SCROLL_THROW_BEGIN",
    14: "SCROLL_END",
    15: "SCROLL",
    16: "GESTURE",
    17: "GESTURE_UP",
    18: "GESTURE_DOWN",
    19: "GESTURE_LEFT",
    20: "GESTURE_RIGHT",
    21: "KEY",
    22: "ROTARY",
    23: "FOCUSED",
    24: "DEFOCUSED",
    25: "LEAVE",
    26: "HIT_TEST",
    27: "INDEV_RESET",
    28: "HOVER_OVER",
    29: "HOVER_LEAVE",
    30: "COVER_CHECK",
    31: "REFR_EXT_DRAW_SIZE",
    32: "DRAW_MAIN_BEGIN",
    33: "DRAW_MAIN",
    34: "DRAW_MAIN_END",
    35: "DRAW_POST_BEGIN",
    36: "DRAW_POST",
    37: "DRAW_POST_END",
    38: "DRAW_TASK_ADDED",
    39: "VALUE_CHANGED",
    40: "INSERT",
    41: "REFRESH",
    42: "READY",
    43: "CANCEL",
    44: "STATE_CHANGED",
    45: "CHECKED",
    46: "UNCHECKED",
    47: "CREATE",
    48: "DELETE",
    49: "CHILD_CHANGED",
    50: "CHILD_CREATED",
    51: "CHILD_DELETED",
    52: "SCREEN_UNLOAD_START",
    53: "SCREEN_LOAD_START",
    54: "SCREEN_LOADED",
    55: "SCREEN_UNLOADED",
    56: "SIZE_CHANGED",
    57: "STYLE_CHANGED",
    58: "LAYOUT_CHANGED",
    59: "GET_SELF_SIZE",
    60: "INVALIDATE_AREA",
    61: "RESOLUTION_CHANGED",
    62: "COLOR_FORMAT_CHANGED",
    63: "REFR_REQUEST",
    64: "REFR_START",
    65: "REFR_READY",
    66: "RENDER_START",
    67: "RENDER_READY",
    68: "FLUSH_START",
    69: "FLUSH_FINISH",
    70: "FLUSH_WAIT_START",
    71: "FLUSH_WAIT_FINISH",
    72: "SYNC_START",
    73: "SYNC_FINISH",
    74: "SYNC_WAIT_START",
    75: "SYNC_WAIT_FINISH",
    76: "UPDATE_LAYOUT_COMPLETED",
    77: "VSYNC",
    78: "VSYNC_REQUEST",
    79: "TRANSLATION_LANGUAGE_CHANGED",
    32767: "LAST_CUSTOM",
}
"""

FILES['lvglgdb/lvgl/misc/lv_fs.py'] = """from lvglgdb.value import Value, ValueInput
from .lv_utils import resolve_source_name, build_global_field_map


class LVFsDrv(Value):
    \"\"\"LVGL filesystem driver wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("letter", lambda d: f"{d['letter']}:"),
            ("type", "driver_name"),
            ("cache_size", "cache_size"),
            ("open_cb", "open_cb"),
            ("read_cb", "read_cb"),
            ("write_cb", "write_cb"),
            ("close_cb", "close_cb"),
        ],
        "table": [],
        "empty_msg": "No registered filesystem drivers.",
    }

    def __init__(self, drv: ValueInput):
        super().__init__(Value.normalize(drv, "lv_fs_drv_t"))

    @property
    def letter(self) -> str:
        return chr(int(self.super_value("letter")))

    # Lazily built address -> name map shared across all instances
    _addr_map = None

    @property
    def driver_name(self) -> str:
        \"\"\"Infer driver name from lv_global_t field or open_cb source file.\"\"\"
        if LVFsDrv._addr_map is None:
            m = build_global_field_map("lv_fs_drv_t")
            LVFsDrv._addr_map = {
                addr: name.replace("_fs_drv", "") for addr, name in m.items()
            }
        # Try lv_global_t field match first (no debug info needed)
        name = LVFsDrv._addr_map.get(int(self), None)
        if name:
            return name
        # Fallback: resolve from open_cb source file (needs debug info)
        return resolve_source_name(int(self.open_cb), prefix="lv_fs_") or "unknown"

    @property
    def cache_size(self) -> int:
        return int(self.super_value("cache_size"))

    @property
    def ready_cb(self) -> Value:
        return self.super_value("ready_cb")

    @property
    def open_cb(self) -> Value:
        return self.super_value("open_cb")

    @property
    def close_cb(self) -> Value:
        return self.super_value("close_cb")

    @property
    def read_cb(self) -> Value:
        return self.super_value("read_cb")

    @property
    def write_cb(self) -> Value:
        return self.super_value("write_cb")

    @property
    def seek_cb(self) -> Value:
        return self.super_value("seek_cb")

    @property
    def tell_cb(self) -> Value:
        return self.super_value("tell_cb")

    @property
    def dir_open_cb(self) -> Value:
        return self.super_value("dir_open_cb")

    @property
    def dir_read_cb(self) -> Value:
        return self.super_value("dir_read_cb")

    @property
    def dir_close_cb(self) -> Value:
        return self.super_value("dir_close_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        d = {
            "addr": hex(int(self)),
            "letter": self.letter,
            "driver_name": self.driver_name,
            "cache_size": self.cache_size,
            "open_cb": fmt_cb(self.open_cb),
            "read_cb": fmt_cb(self.read_cb),
            "write_cb": fmt_cb(self.write_cb),
            "close_cb": fmt_cb(self.close_cb),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(drivers):
        return [drv.snapshot() for drv in drivers]
"""

FILES['lvglgdb/lvgl/misc/lv_image_cache.py'] = """from lvglgdb.value import CorruptedError, Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageCacheData(Value):
    \"\"\"Wrapper for lv_image_cache_data_t with sanity check\"\"\"

    def __init__(self, data_ptr):
        super().__init__(Value.normalize(data_ptr, "lv_image_cache_data_t"))

    def sanity_check(self, entry_addr):
        \"\"\"Validate image cache data fields\"\"\"
        errors = []
        prefix = f"entry {entry_addr:#x}"

        decoded = self.decoded
        if not decoded:
            errors.append(f"{prefix}: null decoded pointer")
            return errors

        header = decoded.header
        w = int(header.w)
        h = int(header.h)
        if w <= 0 or h <= 0:
            errors.append(f"{prefix}: invalid size {w}x{h}")

        data_size = int(decoded.data_size)
        if data_size <= 0:
            errors.append(f"{prefix}: invalid data_size {data_size}")

        src_type = int(self.src_type)
        if src_type not in (0, 1):
            errors.append(f"{prefix}: unknown src_type {src_type}")

        src = self.src
        if not src or int(src) == 0:
            errors.append(f"{prefix}: null src pointer")

        return errors


class LVImageCache(object):
    _DISPLAY_SPEC = {
        "info": [
            ("size", "size"),
            ("data_size", lambda d: str(d["data_size"])),
            ("cf", lambda d: str(d["cf"])),
            ("rc", lambda d: str(d["ref_count"])),
            ("type", "src_type"),
            ("decoder", "decoder_name"),
            ("decoded", "decoded_addr"),
            ("src", "src"),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_cache_data_t")

    def snapshot(self):
        return self._cache.snapshot()

    def snapshots(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        iterator = iter(self._cache)
        result = []

        for entry in iterator:
            data_ptr = entry.get_data()
            if not data_ptr:
                continue

            ref_cnt = 0
            decoded_ptr = 0
            size_str = ""
            data_size = 0
            cf = 0
            decoder_name = ""
            type_str = "unkn"
            src_str = ""

            try:
                ref_cnt = entry.get_ref_count()
                decoded = data_ptr.decoded
                decoded_ptr = int(decoded) if decoded else 0
                src_type = int(data_ptr.src_type)
                src = data_ptr.src

                header = decoded.header
                w = int(header.w)
                h = int(header.h)
                cf = int(header.cf)
                data_size = int(decoded.data_size) if decoded else 0
                size_str = f"{w}x{h}"

                decoder_name = data_ptr.decoder.name.string()

                if src_type == 0:  # LV_IMAGE_SRC_VARIABLE
                    src_str = src.format_string(
                        symbols=True, address=True, styling=True
                    )
                    type_str = "var"
                elif src_type == 1:  # LV_IMAGE_SRC_FILE
                    src_str = (
                        src.cast("char", ptr=True).string() if src else "(null)"
                    )
                    type_str = "file"
                else:
                    src_str = f"{int(src):#x}" if src else "0x0"

            except CorruptedError as e:
                src_str = src_str or str(e)

            extras = dict(zip(iterator.extra_fields, iterator.get_extra(entry)))
            d = {
                "entry_addr": f"{int(entry):#x}",
                "extra_fields": extras,
                "size": size_str,
                "data_size": data_size,
                "cf": cf,
                "ref_count": ref_cnt,
                "src_type": type_str,
                "decoder_name": decoder_name,
                "decoded_addr": f"{decoded_ptr:#x}",
                "src": src_str,
            }
            result.append(Snapshot(d, source=entry,
                                   display_spec=self._DISPLAY_SPEC))

        self._last_extra_fields = iterator.extra_fields
        return result

    @staticmethod
    def _check_image_entry(entry):
        \"\"\"Delegate sanity check to LVImageCacheData\"\"\"
        data_ptr = entry.get_data()
        if not data_ptr:
            return [f"entry {int(entry):#x}: null data pointer"]
        try:
            return LVImageCacheData(data_ptr).sanity_check(int(entry))
        except Exception as e:
            return [f"entry {int(entry):#x}: error: {e}"]

    def sanity_check(self):
        \"\"\"Run sanity check on image cache with image-specific entry validation\"\"\"
        return self._cache.sanity_check(self._check_image_entry)
"""

FILES['lvglgdb/lvgl/misc/lv_image_decoder.py'] = """from lvglgdb.value import Value, ValueInput


class LVImageDecoder(Value):
    \"\"\"LVGL image decoder wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("name", "name"),
            ("info_cb", "info_cb"),
            ("open_cb", "open_cb"),
            ("close_cb", "close_cb"),
        ],
        "table": [],
        "empty_msg": "No registered image decoders.",
    }

    def __init__(self, decoder: ValueInput):
        super().__init__(Value.normalize(decoder, "lv_image_decoder_t"))

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    @property
    def info_cb(self) -> Value:
        return self.super_value("info_cb")

    @property
    def open_cb(self) -> Value:
        return self.super_value("open_cb")

    @property
    def close_cb(self) -> Value:
        return self.super_value("close_cb")

    @property
    def get_area_cb(self) -> Value:
        return self.super_value("get_area_cb")

    @property
    def custom_draw_cb(self) -> Value:
        return self.super_value("custom_draw_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "info_cb": fmt_cb(self.info_cb),
            "open_cb": fmt_cb(self.open_cb),
            "close_cb": fmt_cb(self.close_cb),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(decoders):
        return [dec.snapshot() for dec in decoders]
"""

FILES['lvglgdb/lvgl/misc/lv_image_header_cache.py'] = """from lvglgdb.value import CorruptedError, Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageHeaderCacheData(Value):
    \"\"\"Wrapper for lv_image_header_cache_data_t with sanity check\"\"\"

    def __init__(self, data_ptr):
        super().__init__(Value.normalize(data_ptr, "lv_image_header_cache_data_t"))

    def sanity_check(self, entry_addr):
        \"\"\"Validate image header cache data fields\"\"\"
        errors = []
        prefix = f"entry {entry_addr:#x}"

        header = self.header
        w = int(header.w)
        h = int(header.h)
        if w <= 0 or h <= 0:
            errors.append(f"{prefix}: invalid size {w}x{h}")

        src_type = int(self.src_type)
        if src_type not in (0, 1):
            errors.append(f"{prefix}: unknown src_type {src_type}")

        src = self.src
        if not src or int(src) == 0:
            errors.append(f"{prefix}: null src pointer")

        return errors


class LVImageHeaderCache(object):
    _DISPLAY_SPEC = {
        "info": [
            ("size", "size"),
            ("cf", lambda d: str(d["cf"])),
            ("rc", lambda d: str(d["ref_count"])),
            ("type", "src_type"),
            ("decoder", "decoder_name"),
            ("src", "src"),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_header_cache_data_t")

    def snapshot(self):
        return self._cache.snapshot()

    def snapshots(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        iterator = iter(self._cache)
        result = []

        for entry in iterator:
            data_ptr = entry.get_data()
            if not data_ptr:
                continue

            ref_cnt = 0
            size_str = ""
            cf = 0
            decoder_name = ""
            type_str = "unkn"
            src_str = ""

            try:
                ref_cnt = entry.get_ref_count()
                src_type = int(data_ptr.src_type)
                src = data_ptr.src

                header = data_ptr.header
                w = int(header.w)
                h = int(header.h)
                cf = int(header.cf)
                size_str = f"{w}x{h}"

                decoder_name = data_ptr.decoder.name.string()

                if src_type == 0:  # LV_IMAGE_SRC_VARIABLE
                    src_str = src.format_string(
                        symbols=True, address=True, styling=True
                    )
                    type_str = "var"
                elif src_type == 1:  # LV_IMAGE_SRC_FILE
                    src_str = (
                        src.cast("char", ptr=True).string() if src else "(null)"
                    )
                    type_str = "file"
                else:
                    src_str = f"{int(src):#x}" if src else "0x0"

            except CorruptedError as e:
                src_str = src_str or str(e)

            extras = dict(zip(iterator.extra_fields, iterator.get_extra(entry)))
            d = {
                "entry_addr": f"{int(entry):#x}",
                "extra_fields": extras,
                "size": size_str,
                "cf": cf,
                "ref_count": ref_cnt,
                "src_type": type_str,
                "decoder_name": decoder_name,
                "src": src_str,
            }
            result.append(Snapshot(d, source=entry,
                                   display_spec=self._DISPLAY_SPEC))

        self._last_extra_fields = iterator.extra_fields
        return result

    @staticmethod
    def _check_header_entry(entry):
        \"\"\"Delegate sanity check to LVImageHeaderCacheData\"\"\"
        data_ptr = entry.get_data()
        if not data_ptr:
            return [f"entry {int(entry):#x}: null data pointer"]
        try:
            return LVImageHeaderCacheData(data_ptr).sanity_check(int(entry))
        except Exception as e:
            return [f"entry {int(entry):#x}: error: {e}"]

    def sanity_check(self):
        \"\"\"Run sanity check on image header cache with header-specific entry validation\"\"\"
        return self._cache.sanity_check(self._check_header_entry)
"""

FILES['lvglgdb/lvgl/misc/lv_ll.py'] = """from typing import Union
import gdb

from lvglgdb.value import Value, ValueInput


class LVList(Value):
    \"\"\"LVGL linked list iterator\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Linked List Info:"),
            ("Address", "addr"),
            ("Node Size", "n_size"),
            ("Node Count", "node_count"),
            ("_skip_if", "nodetype", None, ("Node Type", "nodetype")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, ll: ValueInput, nodetype: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(ll, "lv_ll_t"))

        self.nodetype = (
            gdb.lookup_type(nodetype).pointer()
            if isinstance(nodetype, str)
            else nodetype
        )
        self.lv_ll_node_t = gdb.lookup_type("lv_ll_node_t").pointer()
        self.current = self.head
        self._next_offset = self.n_size + self.lv_ll_node_t.sizeof
        self._prev_offset = self.n_size

    def _next(self, node):
        next_value = Value(int(node) + self._next_offset)
        return next_value.cast(self.lv_ll_node_t, ptr=True).dereference()

    def _prev(self, node):
        prev_value = Value(int(node) + self._prev_offset)
        return prev_value.cast(self.lv_ll_node_t, ptr=True).dereference()

    def __iter__(self):
        return self

    def __next__(self):
        if not self.current:
            raise StopIteration

        node = self.current.cast(self.nodetype or self.lv_ll_node_t)
        self.current = self._next(self.current)
        return node

    @property
    def len(self):
        count = 0
        node = self.head
        while node:
            count += 1
            node = self._next(node)
        return count

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "n_size": int(self.n_size),
            "node_count": self.len,
            "nodetype": str(self.nodetype) if self.nodetype else None,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
"""

FILES['lvglgdb/lvgl/misc/lv_rb.py'] = """from typing import Union
import gdb

from lvglgdb.value import CorruptedError, Value, ValueInput


class LVRedBlackTree(Value):
    \"\"\"LVGL red-black tree iterator\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Red-Black Tree Info:"),
            ("Address", "addr"),
            ("Node Size", "size"),
            ("Node Count", "node_count"),
            ("_skip_if", "datatype", None, ("Data Type", "datatype")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, rb: ValueInput, datatype: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(rb, "lv_rb_t"))
        self.lv_rb_node_t = gdb.lookup_type("lv_rb_node_t").pointer()
        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

    def minimum_from(self, node):
        \"\"\"Find minimum node from given node\"\"\"
        if not node:
            return None

        current = node
        while current.left:
            current = current.left
        return current

    def maximum_from(self, node):
        \"\"\"Find maximum node from given node\"\"\"
        if not node:
            return None

        current = node
        while current.right:
            current = current.right
        return current

    def minimum(self):
        \"\"\"Find minimum node in the tree\"\"\"
        root = self.root
        return self.minimum_from(root)

    def maximum(self):
        \"\"\"Find maximum node in the tree\"\"\"
        root = self.root
        return self.maximum_from(root)

    def __iter__(self):
        \"\"\"Create a new iterator for this tree\"\"\"
        return LVRedBlackTreeIterator(self)

    def __len__(self):
        \"\"\"Get the number of nodes in the tree\"\"\"
        count = 0
        for _ in self:
            count += 1
        return count

    def get_data(self, node):
        \"\"\"Get typed data from node, with optional type conversion\"\"\"
        if not node or not node.data:
            return None

        data = node.data
        if self.datatype:
            return data.cast(self.datatype)
        return data

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "size": int(self.size),
            "node_count": len(self),
            "datatype": str(self.datatype) if self.datatype else None,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)


class LVRedBlackTreeIterator:
    \"\"\"Iterator for LVRedBlackTree that supports multiple traversals\"\"\"

    def __init__(self, tree: LVRedBlackTree):
        self.tree = tree
        self.current = tree.minimum()

    def __iter__(self):
        return self

    def __next__(self):
        if not self.current:
            raise StopIteration

        data = self.tree.get_data(self.current)

        # Advance to next node (in-order traversal)
        if self.current.right:
            self.current = self.tree.minimum_from(self.current.right)
        else:
            parent = self.current.parent
            while parent and self.current == parent.right:
                self.current = parent
                parent = parent.parent
            self.current = parent

        if data is None:
            return self.__next__()
        return data

    def __str__(self):
        \"\"\"Better string representation for iterator\"\"\"
        current = self.current
        if not current:
            return "LVRedBlackTreeIterator(ended)"

        try:
            data = self.tree.get_data(current)
            if data:
                return f"LVRedBlackTreeIterator(current={data})"
        except CorruptedError:
            pass

        return f"LVRedBlackTreeIterator(current=0x{int(current):x})"


"""

FILES['lvglgdb/lvgl/misc/lv_style.py'] = """from dataclasses import dataclass
from typing import Iterator

import gdb
from lvglgdb.value import CorruptedError, Value, ValueInput
from .lv_style_consts import (
    STYLE_PROP_NAMES,
    PART_NAMES,
    STATE_FLAGS,
    COLOR_PROPS,
    POINTER_PROPS,
    SRC_PROPS,
    COORD_PROPS,
    BOOL_PROPS,
    ENUM_PROP_VALUES,
    COORD_TYPE_SHIFT,
)

_COORD_TYPE_MASK = 3 << COORD_TYPE_SHIFT
_COORD_TYPE_SPEC = 1 << COORD_TYPE_SHIFT
_COORD_MAX = (1 << COORD_TYPE_SHIFT) - 1
_PCT_STORED_MAX = _COORD_MAX - 1
_PCT_POS_MAX = _PCT_STORED_MAX // 2

# lv_image_src_get_type(): the first byte of an image source says what it is.
_IMAGE_HEADER_MAGIC = 0x19
_IMAGE_HEADER_LEGACY = 0x00

_symbol_cache: dict[int, "str | None"] = {}


def style_prop_name(prop_id: int) -> str:
    \"\"\"Resolve style property ID to human-readable name.\"\"\"
    return STYLE_PROP_NAMES.get(prop_id, f"UNKNOWN({prop_id})")


def decode_selector(selector: int) -> str:
    \"\"\"Decode selector into part + state string.\"\"\"
    part_val = (selector >> 16) & 0xFF
    state_val = selector & 0xFFFF

    part_str = PART_NAMES.get(part_val, f"PART({part_val:#x})")

    if state_val == 0:
        state_str = "DEFAULT"
    elif state_val == 0xFFFF:
        state_str = "ANY"
    else:
        flags = [name for bit, name in STATE_FLAGS.items() if state_val & bit]
        state_str = "|".join(flags) if flags else f"STATE({state_val:#x})"

    return f"{part_str}|{state_str}"


def decode_coord(raw: int) -> str:
    \"\"\"Render an int32 coordinate the way the source wrote it.

    LV_PCT() and LV_SIZE_CONTENT survive in the stored value as a type tag in
    the top bits, so a percentage never has to be inferred from geometry.
    \"\"\"
    bits = raw & 0xFFFFFFFF
    # Only LV_COORD_TYPE_SPEC carries an encoding. LV_COORD_TYPE_PX_NEG is not a
    # second one: there is no LV_COORD_SET_PX_NEG, and in two's complement every
    # negative int32 already has those top bits, so a "PX_NEG" value is just a
    # negative number and prints as itself.
    if bits & _COORD_TYPE_MASK != _COORD_TYPE_SPEC:
        return str(raw)
    plain = bits & ~_COORD_TYPE_MASK
    if plain == _COORD_MAX:
        return "content"
    # Not unreachable: _COORD_TYPE_MASK covers bits 29-30 only, so a negative
    # raw keeps bit 31 in `plain`. LVGL's LV_COORD_IS_PCT() guards the same way.
    if plain > _PCT_STORED_MAX:
        return str(raw)
    pct = plain if plain <= _PCT_POS_MAX else _PCT_POS_MAX - plain
    return f"{pct}%"


def decode_enum(prop_id: int, raw: int) -> str:
    \"\"\"Name an enum value, decomposing bitmasks such as BORDER_SIDE.

    Only single-bit members are combined, and only when they account for every
    set bit. Otherwise a sequential enum like FLEX_FLOW would render an
    out-of-range value as a meaningless list of its low members.
    \"\"\"
    names = ENUM_PROP_VALUES[prop_id]
    if raw in names:
        return names[raw]
    parts, covered = [], 0
    for bit in sorted(b for b in names if b and not b & (b - 1)):
        if raw & bit:
            parts.append(names[bit])
            covered |= bit
    return "|".join(parts) if parts and covered == raw else str(raw)


def read_symbol(addr: int) -> "str | None":
    \"\"\"Resolve an address to its C symbol name, or None.\"\"\"
    if addr not in _symbol_cache:
        try:
            out = gdb.execute(f"info symbol {addr:#x}", to_string=True).strip()
        except gdb.error:
            out = ""
        name = out.split(" in section ")[0] if " in section " in out else None
        _symbol_cache[addr] = name
    return _symbol_cache[addr]


def _forget_symbols(_event=None):
    \"\"\"Addresses move when the program is re-run, so the cache must not outlive it.\"\"\"
    _symbol_cache.clear()


try:
    gdb.events.exited.connect(_forget_symbols)
    gdb.events.new_objfile.connect(_forget_symbols)
except AttributeError:
    pass  # older GDB, or imported outside a debug session


def _read_image_src(addr: int) -> "str | None":
    \"\"\"Read an image source pointer the way lv_image_src_get_type() does.\"\"\"
    try:
        # bytes(): GDB returns a memoryview whose element type differs
        # between versions - b'\\xf3' on some, 243 on others.
        first = bytes(gdb.selected_inferior().read_memory(addr, 1))[0]
        if first == _IMAGE_HEADER_MAGIC or first == _IMAGE_HEADER_LEGACY:
            return None  # an lv_image_dsc_t, so the symbol name is the useful part
        return gdb.Value(addr).cast(gdb.lookup_type("char").pointer()).string()
    except (gdb.error, gdb.MemoryError, UnicodeDecodeError):
        return None


def _format_pointer(prop_id: int, addr: int) -> str:
    \"\"\"Name a pointer property: its symbol, the string it points at, or its address.\"\"\"
    if not addr:
        return "NULL"
    symbol = read_symbol(addr)
    if symbol:
        return symbol
    if prop_id in SRC_PROPS:
        text = _read_image_src(addr)
        if text:
            return text
    return f"{addr:#x}"


def _style_value_data(prop_id: int, value: Value) -> dict:
    \"\"\"Extract style value as pure data dict (no ANSI codes).

    Returns dict with 'value_str', optional 'color_rgb' and, for pointers,
    'ptr' so the raw address is never lost behind a resolved name.
    \"\"\"
    try:
        if prop_id in COLOR_PROPS:
            color = value.color
            r = int(color.red) & 0xFF
            g = int(color.green) & 0xFF
            b = int(color.blue) & 0xFF
            return {
                "value_str": f"#{r:02x}{g:02x}{b:02x}",
                "color_rgb": {"r": r, "g": g, "b": b},
            }
        elif prop_id in POINTER_PROPS:
            ptr = int(value.ptr)
            data = {"value_str": _format_pointer(prop_id, ptr)}
            if ptr:
                data["ptr"] = f"{ptr:#x}"
            return data
        elif prop_id in ENUM_PROP_VALUES:
            return {"value_str": decode_enum(prop_id, int(value.num))}
        elif prop_id in BOOL_PROPS:
            return {"value_str": "true" if int(value.num) else "false"}
        elif prop_id in COORD_PROPS:
            return {"value_str": decode_coord(int(value.num))}
        else:
            return {"value_str": str(int(value.num))}
    except CorruptedError:
        return {"value_str": str(value)}


def format_style_value(prop_id: int, value: Value) -> str:
    \"\"\"Format a style value based on property type (with ANSI color block).\"\"\"
    data = _style_value_data(prop_id, value)
    rgb = data.get("color_rgb")
    if rgb:
        block = f"\\033[48;2;{rgb['r']};{rgb['g']};{rgb['b']}m  \\033[0m"
        return f"{data['value_str']} {block}"
    return data["value_str"]


@dataclass
class StyleEntry:
    \"\"\"A single resolved style property.\"\"\"

    prop_id: int
    value: Value

    @property
    def prop_name(self) -> str:
        return style_prop_name(self.prop_id)

    @property
    def value_str(self) -> str:
        return format_style_value(self.prop_id, self.value)


class LVStyle(Value):
    \"\"\"LVGL style wrapper for lv_style_t.\"\"\"

    def __init__(self, style: ValueInput):
        super().__init__(Value.normalize(style, "lv_style_t"))

    def __iter__(self) -> Iterator[StyleEntry]:
        prop_cnt = int(self.prop_cnt)
        if prop_cnt == 0xFF:
            # Constant style: lv_style_const_prop_t array terminated by prop==0
            const_props = self.values_and_props.cast("lv_style_const_prop_t", ptr=True)
            j = 0
            while True:
                prop_id = int(const_props[j].prop)
                if prop_id == 0 or prop_id == 0xFF:
                    break
                yield StyleEntry(prop_id, const_props[j].value)
                j += 1
        elif prop_cnt > 0:
            # Normal style: values[prop_cnt] then props[prop_cnt]
            # C code: (lv_style_prop_t*)vp + prop_cnt * sizeof(lv_style_value_t)
            # The pointer arithmetic uses lv_style_prop_t element size as stride.
            base = self.values_and_props
            value_t = gdb.lookup_type("lv_style_value_t")
            prop_t = gdb.lookup_type("lv_style_prop_t")
            values_ptr = base.cast(value_t, ptr=True)
            props_offset = prop_cnt * value_t.sizeof * prop_t.sizeof
            props_ptr = Value(int(base) + props_offset).cast(prop_t, ptr=True)

            for j in range(prop_cnt):
                prop_id = int(props_ptr[j])
                if prop_id == 0:
                    continue
                yield StyleEntry(prop_id, values_ptr[j])

    def snapshots(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        result = []
        for entry in self.__iter__():
            vdata = _style_value_data(entry.prop_id, entry.value)
            d = {
                "prop_id": entry.prop_id,
                "prop_name": entry.prop_name,
                **vdata,
            }
            result.append(Snapshot(d, source=entry))
        return result


"""

FILES['lvglgdb/lvgl/misc/lv_style_consts.py'] = """\"\"\"
Auto-generated style constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

STYLE_PROP_NAMES = {
    1: "WIDTH",
    2: "HEIGHT",
    3: "LENGTH",
    4: "TRANSFORM_WIDTH",
    5: "TRANSFORM_HEIGHT",
    8: "MIN_WIDTH",
    9: "MAX_WIDTH",
    10: "MIN_HEIGHT",
    11: "MAX_HEIGHT",
    12: "TRANSLATE_X",
    13: "TRANSLATE_Y",
    14: "RADIAL_OFFSET",
    16: "X",
    17: "Y",
    18: "ALIGN",
    24: "PAD_TOP",
    25: "PAD_BOTTOM",
    26: "PAD_LEFT",
    27: "PAD_RIGHT",
    28: "PAD_RADIAL",
    29: "PAD_ROW",
    30: "PAD_COLUMN",
    32: "MARGIN_TOP",
    33: "MARGIN_BOTTOM",
    34: "MARGIN_LEFT",
    35: "MARGIN_RIGHT",
    40: "BG_GRAD",
    41: "BG_GRAD_DIR",
    42: "BG_MAIN_OPA",
    43: "BG_GRAD_OPA",
    44: "BG_GRAD_COLOR",
    45: "BG_MAIN_STOP",
    46: "BG_GRAD_STOP",
    48: "BG_IMAGE_SRC",
    49: "BG_IMAGE_OPA",
    50: "BG_IMAGE_RECOLOR_OPA",
    51: "BG_IMAGE_TILED",
    52: "BG_IMAGE_RECOLOR",
    56: "BORDER_WIDTH",
    57: "BORDER_COLOR",
    58: "BORDER_OPA",
    59: "BORDER_POST",
    60: "BORDER_SIDE",
    64: "OUTLINE_WIDTH",
    65: "OUTLINE_COLOR",
    66: "OUTLINE_OPA",
    67: "OUTLINE_PAD",
    72: "BG_OPA",
    73: "BG_COLOR",
    74: "SHADOW_WIDTH",
    75: "LINE_WIDTH",
    76: "ARC_WIDTH",
    77: "TEXT_FONT",
    78: "IMAGE_RECOLOR_OPA",
    80: "IMAGE_OPA",
    81: "SHADOW_OPA",
    82: "LINE_OPA",
    83: "ARC_OPA",
    84: "TEXT_OPA",
    88: "SHADOW_COLOR",
    89: "IMAGE_RECOLOR",
    90: "LINE_COLOR",
    91: "ARC_COLOR",
    92: "TEXT_COLOR",
    96: "ARC_IMAGE_SRC",
    97: "SHADOW_OFFSET_X",
    98: "SHADOW_OFFSET_Y",
    99: "SHADOW_SPREAD",
    100: "LINE_DASH_WIDTH",
    101: "TEXT_ALIGN",
    102: "TEXT_LETTER_SPACE",
    103: "TEXT_LINE_SPACE",
    104: "LINE_DASH_GAP",
    105: "LINE_ROUNDED",
    106: "IMAGE_COLORKEY",
    107: "TEXT_OUTLINE_STROKE_WIDTH",
    108: "TEXT_OUTLINE_STROKE_OPA",
    109: "TEXT_OUTLINE_STROKE_COLOR",
    110: "TEXT_DECOR",
    111: "ARC_ROUNDED",
    112: "OPA",
    113: "OPA_LAYERED",
    114: "COLOR_FILTER_DSC",
    115: "COLOR_FILTER_OPA",
    116: "ANIM",
    117: "ANIM_DURATION",
    118: "TRANSITION",
    120: "RADIUS",
    121: "BITMAP_MASK_SRC",
    122: "BLEND_MODE",
    123: "ROTARY_SENSITIVITY",
    124: "TRANSLATE_RADIAL",
    128: "CLIP_CORNER",
    129: "BASE_DIR",
    130: "RECOLOR",
    131: "RECOLOR_OPA",
    132: "LAYOUT",
    136: "BLUR_RADIUS",
    137: "BLUR_BACKDROP",
    138: "BLUR_QUALITY",
    144: "DROP_SHADOW_RADIUS",
    145: "DROP_SHADOW_OFFSET_X",
    146: "DROP_SHADOW_OFFSET_Y",
    147: "DROP_SHADOW_COLOR",
    148: "DROP_SHADOW_OPA",
    149: "DROP_SHADOW_QUALITY",
    152: "TRANSFORM_SCALE_X",
    153: "TRANSFORM_SCALE_Y",
    154: "TRANSFORM_PIVOT_X",
    155: "TRANSFORM_PIVOT_Y",
    156: "TRANSFORM_ROTATION",
    157: "TRANSFORM_SKEW_X",
    158: "TRANSFORM_SKEW_Y",
    160: "FLEX_FLOW",
    161: "FLEX_MAIN_PLACE",
    162: "FLEX_CROSS_PLACE",
    163: "FLEX_TRACK_PLACE",
    164: "FLEX_GROW",
    165: "GRID_COLUMN_DSC_ARRAY",
    166: "GRID_ROW_DSC_ARRAY",
    168: "GRID_COLUMN_ALIGN",
    169: "GRID_ROW_ALIGN",
    170: "GRID_CELL_COLUMN_POS",
    171: "GRID_CELL_COLUMN_SPAN",
    172: "GRID_CELL_X_ALIGN",
    173: "GRID_CELL_ROW_POS",
    174: "GRID_CELL_ROW_SPAN",
    175: "GRID_CELL_Y_ALIGN",
    176: "TEXT_LEADING_TRIM",
}

PART_NAMES = {
    0x00: "MAIN",
    0x01: "SCROLLBAR",
    0x02: "INDICATOR",
    0x03: "KNOB",
    0x04: "SELECTED",
    0x05: "ITEMS",
    0x06: "CURSOR",
    0x08: "CUSTOM_FIRST",
    0x0F: "ANY",
}

STATE_FLAGS = {
    0x0001: "ALT",
    0x0004: "CHECKED",
    0x0008: "FOCUSED",
    0x0010: "FOCUS_KEY",
    0x0020: "EDITED",
    0x0040: "HOVERED",
    0x0080: "PRESSED",
    0x0100: "SCROLLED",
    0x0200: "DISABLED",
    0x1000: "USER_1",
    0x2000: "USER_2",
    0x4000: "USER_3",
    0x8000: "USER_4",
}

COLOR_PROPS = {
    44,  # BG_GRAD_COLOR
    52,  # BG_IMAGE_RECOLOR
    57,  # BORDER_COLOR
    65,  # OUTLINE_COLOR
    73,  # BG_COLOR
    88,  # SHADOW_COLOR
    89,  # IMAGE_RECOLOR
    90,  # LINE_COLOR
    91,  # ARC_COLOR
    92,  # TEXT_COLOR
    109,  # TEXT_OUTLINE_STROKE_COLOR
    130,  # RECOLOR
    147,  # DROP_SHADOW_COLOR
}

POINTER_PROPS = {
    40,  # BG_GRAD
    48,  # BG_IMAGE_SRC
    77,  # TEXT_FONT
    96,  # ARC_IMAGE_SRC
    106,  # IMAGE_COLORKEY
    114,  # COLOR_FILTER_DSC
    116,  # ANIM
    118,  # TRANSITION
    121,  # BITMAP_MASK_SRC
    165,  # GRID_COLUMN_DSC_ARRAY
    166,  # GRID_ROW_DSC_ARRAY
}

SRC_PROPS = {
    48,  # BG_IMAGE_SRC
    96,  # ARC_IMAGE_SRC
    121,  # BITMAP_MASK_SRC
}

COORD_PROPS = {
    1,  # WIDTH
    2,  # HEIGHT
    3,  # LENGTH
    4,  # TRANSFORM_WIDTH
    5,  # TRANSFORM_HEIGHT
    8,  # MIN_WIDTH
    9,  # MAX_WIDTH
    10,  # MIN_HEIGHT
    11,  # MAX_HEIGHT
    12,  # TRANSLATE_X
    13,  # TRANSLATE_Y
    14,  # RADIAL_OFFSET
    16,  # X
    17,  # Y
    24,  # PAD_TOP
    25,  # PAD_BOTTOM
    26,  # PAD_LEFT
    27,  # PAD_RIGHT
    28,  # PAD_RADIAL
    29,  # PAD_ROW
    30,  # PAD_COLUMN
    32,  # MARGIN_TOP
    33,  # MARGIN_BOTTOM
    34,  # MARGIN_LEFT
    35,  # MARGIN_RIGHT
    45,  # BG_MAIN_STOP
    46,  # BG_GRAD_STOP
    56,  # BORDER_WIDTH
    64,  # OUTLINE_WIDTH
    67,  # OUTLINE_PAD
    74,  # SHADOW_WIDTH
    75,  # LINE_WIDTH
    76,  # ARC_WIDTH
    97,  # SHADOW_OFFSET_X
    98,  # SHADOW_OFFSET_Y
    99,  # SHADOW_SPREAD
    100,  # LINE_DASH_WIDTH
    102,  # TEXT_LETTER_SPACE
    103,  # TEXT_LINE_SPACE
    104,  # LINE_DASH_GAP
    107,  # TEXT_OUTLINE_STROKE_WIDTH
    120,  # RADIUS
    124,  # TRANSLATE_RADIAL
    136,  # BLUR_RADIUS
    144,  # DROP_SHADOW_RADIUS
    145,  # DROP_SHADOW_OFFSET_X
    146,  # DROP_SHADOW_OFFSET_Y
    152,  # TRANSFORM_SCALE_X
    153,  # TRANSFORM_SCALE_Y
    154,  # TRANSFORM_PIVOT_X
    155,  # TRANSFORM_PIVOT_Y
    156,  # TRANSFORM_ROTATION
    157,  # TRANSFORM_SKEW_X
    158,  # TRANSFORM_SKEW_Y
    170,  # GRID_CELL_COLUMN_POS
    171,  # GRID_CELL_COLUMN_SPAN
    173,  # GRID_CELL_ROW_POS
    174,  # GRID_CELL_ROW_SPAN
}

BOOL_PROPS = {
    51,  # BG_IMAGE_TILED
    59,  # BORDER_POST
    105,  # LINE_ROUNDED
    111,  # ARC_ROUNDED
    128,  # CLIP_CORNER
    137,  # BLUR_BACKDROP
}

ENUM_PROP_VALUES = {
    18: {  # ALIGN
        0: "DEFAULT",
        1: "TOP_LEFT",
        2: "TOP_MID",
        3: "TOP_RIGHT",
        4: "BOTTOM_LEFT",
        5: "BOTTOM_MID",
        6: "BOTTOM_RIGHT",
        7: "LEFT_MID",
        8: "RIGHT_MID",
        9: "CENTER",
        10: "OUT_TOP_LEFT",
        11: "OUT_TOP_MID",
        12: "OUT_TOP_RIGHT",
        13: "OUT_BOTTOM_LEFT",
        14: "OUT_BOTTOM_MID",
        15: "OUT_BOTTOM_RIGHT",
        16: "OUT_LEFT_TOP",
        17: "OUT_LEFT_MID",
        18: "OUT_LEFT_BOTTOM",
        19: "OUT_RIGHT_TOP",
        20: "OUT_RIGHT_MID",
        21: "OUT_RIGHT_BOTTOM",
    },
    41: {  # BG_GRAD_DIR
        0: "NONE",
        1: "VER",
        2: "HOR",
        3: "LINEAR",
        4: "RADIAL",
        5: "CONICAL",
    },
    60: {  # BORDER_SIDE
        0: "NONE",
        1: "BOTTOM",
        2: "TOP",
        4: "LEFT",
        8: "RIGHT",
        15: "FULL",
        16: "INTERNAL",
    },
    101: {  # TEXT_ALIGN
        0: "AUTO",
        1: "LEFT",
        2: "CENTER",
        3: "RIGHT",
    },
    110: {  # TEXT_DECOR
        0: "NONE",
        1: "UNDERLINE",
        2: "STRIKETHROUGH",
    },
    122: {  # BLEND_MODE
        0: "NORMAL",
        1: "ADDITIVE",
        2: "SUBTRACTIVE",
        3: "MULTIPLY",
        4: "DIFFERENCE",
    },
    129: {  # BASE_DIR
        0: "LTR",
        1: "RTL",
        2: "AUTO",
        32: "NEUTRAL",
        33: "WEAK",
    },
    138: {  # BLUR_QUALITY
        0: "AUTO",
        1: "SPEED",
        2: "PRECISION",
    },
    149: {  # DROP_SHADOW_QUALITY
        0: "AUTO",
        1: "SPEED",
        2: "PRECISION",
    },
    160: {  # FLEX_FLOW
        0: "ROW",
        1: "COLUMN",
        4: "ROW_WRAP",
        5: "COLUMN_WRAP",
        8: "ROW_REVERSE",
        9: "COLUMN_REVERSE",
        12: "ROW_WRAP_REVERSE",
        13: "COLUMN_WRAP_REVERSE",
    },
    161: {  # FLEX_MAIN_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    162: {  # FLEX_CROSS_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    163: {  # FLEX_TRACK_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    168: {  # GRID_COLUMN_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    169: {  # GRID_ROW_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    172: {  # GRID_CELL_X_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    175: {  # GRID_CELL_Y_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    176: {  # TEXT_LEADING_TRIM
        0: "NONE",
        1: "CAPITAL_BASELINE",
        2: "LOWER_BASELINE",
        3: "CAPITAL",
        4: "LOWER",
    },
}

# From LV_COORD_TYPE_SHIFT in lv_area.h
COORD_TYPE_SHIFT = 29
"""

FILES['lvglgdb/lvgl/misc/lv_timer.py'] = """from lvglgdb.value import Value, ValueInput


class LVTimer(Value):
    \"\"\"LVGL timer wrapper\"\"\"

    _DISPLAY_SPEC = {
        "info": [
            ("callback", "timer_cb"),
            ("period", "period"),
            ("freq", "frequency"),
            ("last_run", "last_run"),
            ("repeat", lambda d: (
                "inf" if d["repeat_count"] == -1
                else str(d["repeat_count"])
            )),
            ("paused", "paused"),
        ],
        "table": [],
        "empty_msg": "No active timers.",
    }

    def __init__(self, timer: ValueInput):
        super().__init__(Value.normalize(timer, "lv_timer_t"))

    @property
    def period(self) -> int:
        return int(self.super_value("period"))

    @property
    def last_run(self) -> int:
        return int(self.super_value("last_run"))

    @property
    def timer_cb(self) -> Value:
        return self.super_value("timer_cb")

    @property
    def repeat_count(self) -> int:
        return int(self.super_value("repeat_count"))

    @property
    def paused(self) -> bool:
        return bool(int(self.super_value("paused")))

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def auto_delete(self) -> bool:
        return bool(int(self.super_value("auto_delete")))

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        freq = f"{1000 / self.period:.1f}Hz" if self.period > 0 else "-"
        d = {
            "addr": hex(int(self)),
            "timer_cb": fmt_cb(self.timer_cb),
            "period": self.period,
            "frequency": freq,
            "last_run": self.last_run,
            "repeat_count": self.repeat_count,
            "paused": self.paused,
            "user_data": str(self.user_data),
            "user_data_addr": ptr_or_none(self.user_data),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(timers):
        return [timer.snapshot() for timer in timers]
"""

FILES['lvglgdb/lvgl/misc/lv_utils.py'] = """import os

import gdb


def resolve_source_name(addr, prefix="lv_"):
    \"\"\"Resolve a code address to a short name derived from its source file.

    Tries block_for_pc first, then find_pc_line as fallback.
    Strips common prefixes (e.g. "lv_fs_stdio.c" -> "stdio").

    Args:
        addr: Integer address of a function/callback.
        prefix: Prefix to strip from filename (default "lv_").

    Returns:
        Short name string, or None if resolution fails.
    \"\"\"
    if not addr:
        return None
    fname = None
    try:
        block = gdb.block_for_pc(addr)
        if block and block.function and block.function.symtab:
            fname = block.function.symtab.filename
    except (gdb.error, RuntimeError):
        pass
    if not fname:
        try:
            sal = gdb.find_pc_line(addr)
            if sal and sal.symtab:
                fname = sal.symtab.filename
        except gdb.error:
            pass
    if not fname:
        return None
    base = os.path.basename(fname).replace(".c", "")
    if prefix and base.startswith(prefix):
        base = base[len(prefix) :]
    return base


def build_global_field_map(field_type_name):
    \"\"\"Build address -> field name map for fields of a given type in lv_global_t.

    Scans lv_global_t struct fields, finds those matching the given type,
    and returns a dict mapping their runtime addresses to field names.

    Args:
        field_type_name: C type name string (e.g. "lv_fs_drv_t").

    Returns:
        Dict mapping int address -> str field name.
    \"\"\"
    try:
        lv_global_val = gdb.parse_and_eval("lv_global")
        lv_global_addr = int(lv_global_val.address)
        lv_global_type = gdb.lookup_type("lv_global_t")
        target_type = gdb.lookup_type(field_type_name)
        result = {}
        for field in lv_global_type.fields():
            if field.type.strip_typedefs() == target_type:
                addr = lv_global_addr + field.bitpos // 8
                result[addr] = field.name
        return result
    except gdb.error:
        return {}


# LVGL coordinate type constants (from lv_area.h)
_COORD_TYPE_SHIFT = 29
_COORD_TYPE_MASK = 3 << _COORD_TYPE_SHIFT
_COORD_TYPE_PX = 0 << _COORD_TYPE_SHIFT
_COORD_TYPE_SPEC = 1 << _COORD_TYPE_SHIFT
_COORD_TYPE_PX_NEG = 3 << _COORD_TYPE_SHIFT
_COORD_MAX = (1 << _COORD_TYPE_SHIFT) - 1
_SIZE_CONTENT = _COORD_MAX | _COORD_TYPE_SPEC
_PCT_POS_MAX = (_COORD_MAX - 1) // 2


def format_coord(val):
    \"\"\"Format an lv_coord_t value into a human-readable string.

    Decodes special LVGL coordinate encodings:
      - LV_SIZE_CONTENT       -> "CONTENT"
      - LV_PCT(x)             -> "x%"
      - LV_COORD_TYPE_PX_NEG  -> negative pixel
      - plain pixel            -> "123"
    \"\"\"
    val = int(val)
    if val == _SIZE_CONTENT:
        return "CONTENT"
    coord_type = val & _COORD_TYPE_MASK
    if coord_type == _COORD_TYPE_SPEC:
        plain = val & ~_COORD_TYPE_MASK
        if plain <= _PCT_POS_MAX:
            return f"{plain}%"
        return f"{_PCT_POS_MAX - plain}%"
    # LV_COORD_TYPE_PX_NEG needs no branch of its own. There is no
    # LV_COORD_SET_PX_NEG: the tag exists so LV_COORD_IS_PX() accepts negatives,
    # and in two's complement every negative int32 already has those top bits,
    # so such a value is an ordinary negative pixel count. Taking
    # LV_COORD_PLAIN() of it and negating turned -5 into -536870907.
    return str(val)


_enum_cache = {}


def _enum_members(type_name):
    \"\"\"Members of a C enum as {value: name}, or None if the target has no such type.\"\"\"
    if type_name not in _enum_cache:
        try:
            members = {
                int(f.enumval): f.name for f in gdb.lookup_type(type_name).fields()
            }
        except (gdb.error, AttributeError, TypeError):
            members = None
        _enum_cache[type_name] = members
    return _enum_cache[type_name]


def format_enum(type_name, value, prefix="", bitmask=False):
    \"\"\"Name a value of a C enum, reading the names from the target's debug info.

    The fields holding these values are plain unsigned bitfields, so GDB prints
    them as bare numbers and the enum has to be looked up by name. Reading it
    from the target rather than from a generated table means the names always
    match the binary in front of you - and when the enum was optimised out,
    the raw number is still printed.
    \"\"\"
    if value is None:
        return None
    value = int(value)
    members = _enum_members(type_name)
    if not members:
        return str(value)

    def _short(name):
        return name[len(prefix):] if prefix and name.startswith(prefix) else name

    if value in members:
        return _short(members[value])
    if not bitmask:
        return str(value)

    parts, covered = [], 0
    for bit in sorted(b for b in members if b and not b & (b - 1)):
        if value & bit:
            parts.append(_short(members[bit]))
            covered |= bit
    return "|".join(parts) if parts and covered == value else str(value)


def _forget_enums(_event=None):
    \"\"\"Enum layouts belong to the objfile they were read from.\"\"\"
    _enum_cache.clear()


try:
    gdb.events.exited.connect(_forget_enums)
    gdb.events.new_objfile.connect(_forget_enums)
except AttributeError:
    pass  # older GDB, or imported outside a debug session
"""

FILES['lvglgdb/lvgl/snapshot.py'] = """import functools
from typing import Any, Callable, Dict, Iterator, Optional


class Snapshot:
    \"\"\"Self-describing data snapshot from a wrapper instance.

    Holds a pure Python dict (JSON-serializable), an optional reference
    to the original wrapper (_source), and an optional display spec that
    describes how to format the data for terminal output.

    A snapshot may be marked *corrupted* to indicate that the underlying
    data could not be fully read (e.g. inaccessible target memory).
    \"\"\"

    __slots__ = ("_data", "_source", "_display_spec")

    def __init__(self, data: Dict[str, Any], source: Any = None,
                 display_spec: Optional[Dict] = None):
        self._data = data
        self._source = source
        self._display_spec = display_spec

    @classmethod
    def fallback(cls, addr: Callable = lambda x: int(x), **extra_fns):
        \"\"\"Decorator: on CorruptedError, return a corrupted snapshot dict.

        Each extra kwarg may be a callable (called with the first arg)
        or a static value.
        \"\"\"
        def decorator(fn):
            @functools.wraps(fn)
            def wrapper(item, *args, **kwargs):
                from ..value import CorruptedError
                try:
                    return fn(item, *args, **kwargs)
                except CorruptedError as e:
                    try:
                        resolved_addr = hex(addr(item))
                    except Exception:
                        resolved_addr = "0x?"
                    extra = {}
                    for k, v in extra_fns.items():
                        try:
                            extra[k] = v(item) if callable(v) else v
                        except Exception:
                            extra[k] = None
                    d = {"addr": resolved_addr,
                         "class_name": "(corrupted)",
                         "error": str(e)}
                    if extra:
                        d.update(extra)
                    return d
            return wrapper
        return decorator

    @staticmethod
    def safe_fields(source, field_specs: list) -> dict:
        \"\"\"Collect fields with per-field CorruptedError handling.

        field_specs: list of (key, callable) or (key, callable, default)
        \"\"\"
        from ..value import CorruptedError

        d = {}
        for spec in field_specs:
            key, fn = spec[0], spec[1]
            default = spec[2] if len(spec) > 2 else None
            try:
                d[key] = fn(source)
            except CorruptedError:
                d[key] = default
        return d

    # --- dict-like read access ---

    def __getitem__(self, key: str) -> Any:
        return self._data[key]

    def __setitem__(self, key: str, value: Any) -> None:
        self._data[key] = value

    def __contains__(self, key: str) -> bool:
        return key in self._data

    def __len__(self) -> int:
        return len(self._data)

    def __iter__(self) -> Iterator[str]:
        return iter(self._data)

    def keys(self):
        return self._data.keys()

    def values(self):
        return self._data.values()

    def items(self):
        return self._data.items()

    def get(self, key: str, default: Any = None) -> Any:
        return self._data.get(key, default)

    # --- serialization ---

    def as_dict(self) -> Dict[str, Any]:
        return dict(self._data)

    def __repr__(self) -> str:
        addr = self._data.get("addr", "?")
        src_type = type(self._source).__name__ if self._source else "None"
        return f"Snapshot(addr={addr}, source={src_type})"
"""

FILES['lvglgdb/lvgl/text_table.py'] = """\"\"\"A small ASCII table renderer.

It draws the same boxes prettytable does by default, so the plugin does not
need prettytable installed in the Python that GDB embeds:

    +---+-------+
    | # | name  |
    +---+-------+
    | 0 | timer |
    +---+-------+

Cells may hold several lines, and wide (CJK) characters count as two columns.
\"\"\"

import unicodedata
from typing import Dict, List, Sequence


def _char_width(ch: str) -> int:
    if unicodedata.combining(ch) or unicodedata.category(ch) in ("Mn", "Me", "Cf"):
        return 0
    if unicodedata.east_asian_width(ch) in ("W", "F"):
        return 2
    return 1


def text_width(text: str) -> int:
    \"\"\"Width of one line of text in terminal columns.\"\"\"
    if text.isascii():
        return len(text)
    return sum(_char_width(ch) for ch in text)


def _justify(text: str, width: int, align: str) -> str:
    excess = width - text_width(text)
    if align == "l":
        return text + " " * excess
    if align == "r":
        return " " * excess + text
    # Centre the way str.center() does: the odd space goes to the right of
    # odd-length text and to the left of even-length text.
    left = excess // 2
    if excess % 2 and text_width(text) % 2 == 0:
        left += 1
    return " " * left + text + " " * (excess - left)


class TextTable:
    \"\"\"Collects rows and renders them as an ASCII table.

    `align` is the default alignment of every column ("l", "c" or "r");
    `col_align` overrides it per column name.
    \"\"\"

    def __init__(self, field_names: Sequence[str], align: str = "c",
                 col_align: Dict[str, str] = None):
        self.field_names = [str(name) for name in field_names]
        self.align = {name: align for name in self.field_names}
        for name, a in (col_align or {}).items():
            if name in self.align:
                self.align[name] = a
        self.rows: List[List[str]] = []

    def add_row(self, row: Sequence) -> None:
        if len(row) != len(self.field_names):
            raise ValueError(
                f"Row has {len(row)} cells, but the table has"
                f" {len(self.field_names)} columns"
            )
        self.rows.append([str(cell) for cell in row])

    def __str__(self) -> str:
        names = self.field_names
        widths = [text_width(name) for name in names]
        for row in self.rows:
            for i, cell in enumerate(row):
                for line in cell.split("\\n"):
                    widths[i] = max(widths[i], text_width(line))

        rule = "+" + "+".join("-" * (w + 2) for w in widths) + "+"

        def render(cells: Sequence[str]) -> List[str]:
            split = [cell.split("\\n") for cell in cells]
            height = max(len(lines) for lines in split)
            out = []
            for n in range(height):
                parts = []
                for i, lines in enumerate(split):
                    text = lines[n] if n < len(lines) else ""
                    parts.append(_justify(text, widths[i], self.align[names[i]]))
                out.append("| " + " | ".join(parts) + " |")
            return out

        lines = [rule, *render(names), rule]
        if self.rows:
            for row in self.rows:
                lines.extend(render(row))
            lines.append(rule)
        return "\\n".join(lines)
"""

FILES['lvglgdb/lvgl/widgets/__init__.py'] = """\"\"\"
Auto-generated LVGL widget wrappers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_3dtexture import LV3dtexture
from .lv_image import LVImage
from .lv_animimg import LVAnimimg
from .lv_arc import LVArc
from .lv_arclabel import LVArclabel
from .lv_bar import LVBar
from .lv_canvas import LVCanvas
from .lv_barcode import LVBarcode
from .lv_button import LVButton
from .lv_buttonmatrix import LVButtonmatrix
from .lv_calendar import LVCalendar
from .lv_chart import LVChart
from .lv_checkbox import LVCheckbox
from .lv_dropdown import LVDropdown
from .lv_dropdown_list import LVDropdownList
from .lv_ffmpeg_player import LVFfmpegPlayer
from .lv_gstreamer import LVGstreamer
from .lv_imagebutton import LVImagebutton
from .lv_ime_pinyin import LVImePinyin
from .lv_keyboard import LVKeyboard
from .lv_label import LVLabel
from .lv_led import LVLed
from .lv_line import LVLine
from .lv_menu import LVMenu
from .lv_menu_page import LVMenuPage
from .lv_msgbox import LVMsgbox
from .lv_qrcode import LVQrcode
from .lv_roller import LVRoller
from .lv_scale import LVScale
from .lv_slider import LVSlider
from .lv_spangroup import LVSpangroup
from .lv_textarea import LVTextarea
from .lv_spinbox import LVSpinbox
from .lv_spinner import LVSpinner
from .lv_switch import LVSwitch
from .lv_table import LVTable
from .lv_tabview import LVTabview
from .lv_tileview import LVTileview
from .lv_tileview_tile import LVTileviewTile
from .lv_win import LVWin

WIDGET_REGISTRY: dict[str, type] = {
    "lv_3dtexture": LV3dtexture,
    "lv_image": LVImage,
    "lv_animimg": LVAnimimg,
    "lv_arc": LVArc,
    "lv_arclabel": LVArclabel,
    "lv_bar": LVBar,
    "lv_canvas": LVCanvas,
    "lv_barcode": LVBarcode,
    "lv_button": LVButton,
    "lv_buttonmatrix": LVButtonmatrix,
    "lv_calendar": LVCalendar,
    "lv_chart": LVChart,
    "lv_checkbox": LVCheckbox,
    "lv_dropdown": LVDropdown,
    "lv_dropdown_list": LVDropdownList,
    "lv_ffmpeg_player": LVFfmpegPlayer,
    "lv_gstreamer": LVGstreamer,
    "lv_imagebutton": LVImagebutton,
    "lv_ime_pinyin": LVImePinyin,
    "lv_keyboard": LVKeyboard,
    "lv_label": LVLabel,
    "lv_led": LVLed,
    "lv_line": LVLine,
    "lv_menu": LVMenu,
    "lv_menu_page": LVMenuPage,
    "lv_msgbox": LVMsgbox,
    "lv_qrcode": LVQrcode,
    "lv_roller": LVRoller,
    "lv_scale": LVScale,
    "lv_slider": LVSlider,
    "lv_spangroup": LVSpangroup,
    "lv_textarea": LVTextarea,
    "lv_spinbox": LVSpinbox,
    "lv_spinner": LVSpinner,
    "lv_switch": LVSwitch,
    "lv_table": LVTable,
    "lv_tabview": LVTabview,
    "lv_tileview": LVTileview,
    "lv_tileview_tile": LVTileviewTile,
    "lv_win": LVWin,
}


import gdb


def wrap_widget(obj):
    \"\"\"Wrap an LVObject into its widget class if known.\"\"\"
    name = obj.class_name
    # Before 6d999331d (Feb 2025), LVGL class names had no 'lv_' prefix
    # (e.g. 'label' instead of 'lv_label'). Try both for compatibility.
    cls = WIDGET_REGISTRY.get(name) or WIDGET_REGISTRY.get('lv_' + name)
    if cls:
        try:
            return cls(obj)
        except gdb.error:
            pass  # type not available in debug info
    return None

__all__ = [
    "LV3dtexture",
    "LVImage",
    "LVAnimimg",
    "LVArc",
    "LVArclabel",
    "LVBar",
    "LVCanvas",
    "LVBarcode",
    "LVButton",
    "LVButtonmatrix",
    "LVCalendar",
    "LVChart",
    "LVCheckbox",
    "LVDropdown",
    "LVDropdownList",
    "LVFfmpegPlayer",
    "LVGstreamer",
    "LVImagebutton",
    "LVImePinyin",
    "LVKeyboard",
    "LVLabel",
    "LVLed",
    "LVLine",
    "LVMenu",
    "LVMenuPage",
    "LVMsgbox",
    "LVQrcode",
    "LVRoller",
    "LVScale",
    "LVSlider",
    "LVSpangroup",
    "LVTextarea",
    "LVSpinbox",
    "LVSpinner",
    "LVSwitch",
    "LVTable",
    "LVTabview",
    "LVTileview",
    "LVTileviewTile",
    "LVWin",
    "WIDGET_REGISTRY",
    "wrap_widget",
]
"""

FILES['lvglgdb/lvgl/widgets/_helpers.py'] = """\"\"\"Shared field-reading helpers for widget wrappers.

All helpers return a safe value (or None) without raising exceptions.
Protection is provided by Value.safe_field() and Value.string(fallback=).
\"\"\"

from lvglgdb.lvgl.data_utils import ptr_or_none  # noqa: F401


def safe_string(obj, field_name):
    \"\"\"Read a char* field as string, corrupted marker, or None (NULL/missing).\"\"\"
    from lvglgdb.value import CorruptedValue
    val = obj.safe_field(field_name)
    if val is None:
        return None
    if not getattr(val, 'is_ok', True):
        return str(val)
    addr = int(val)
    if not addr:
        return None
    return val.string(fallback=str(CorruptedValue(addr, MemoryError("unreadable"))))


def safe_color(obj, field_name):
    \"\"\"Read lv_color_t {red, green, blue} as hex string.\"\"\"
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    r = int(val.safe_field("red", 0))
    g = int(val.safe_field("green", 0))
    b = int(val.safe_field("blue", 0))
    return f"#{r:02x}{g:02x}{b:02x}"


def safe_area(obj, field_name):
    \"\"\"Read lv_area_t as dict.\"\"\"
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x1": int(val.safe_field("x1", 0)),
        "y1": int(val.safe_field("y1", 0)),
        "x2": int(val.safe_field("x2", 0)),
        "y2": int(val.safe_field("y2", 0)),
    }


def safe_point(obj, field_name):
    \"\"\"Read lv_point_t as dict.\"\"\"
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x": int(val.safe_field("x", 0)),
        "y": int(val.safe_field("y", 0)),
    }


def safe_wrapper(obj, field_name, module_path, class_name):
    \"\"\"Read a struct field using its known Value wrapper, return snapshot dict.

    Not every wrapper has a snapshot(); LVArray does not. Value forwards an
    unknown attribute to gdb.Value.__getitem__, so asking for one raises
    gdb.error rather than AttributeError, and the caller would otherwise lose
    the whole widget - and its subtree - over a single field.
    \"\"\"
    import gdb

    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    import importlib
    try:
        mod = importlib.import_module(module_path)
        cls = getattr(mod, class_name)
        wrapper = cls(val)
        if getattr(type(wrapper), "snapshot", None) is None:
            # An embedded struct - lv_scale_t.needles - is not convertible to an
            # int, so the address has to come from the field itself.
            if val.type.strip_typedefs().code != gdb.TYPE_CODE_PTR:
                val = val.address
            return {"addr": hex(int(val)), "type": class_name}
        return wrapper.snapshot().as_dict()
    except (gdb.error, gdb.MemoryError, ImportError, AttributeError, TypeError):
        return None
"""

FILES['lvglgdb/lvgl/widgets/lv_3dtexture.py'] = """\"\"\"
Auto-generated wrapper for lv_3dtexture_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LV3dtexture(LVObject):
    \"\"\"LVGL 3dtexture widget (lv_3dtexture_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_3dtexture_t = self.cast("lv_3dtexture_t", ptr=True) or self
        self._wv = self._wv_lv_3dtexture_t

    @property
    def id(self):
        return int(self._wv_lv_3dtexture_t.safe_field("id", 0))

    @property
    def h_flip(self):
        return int(self._wv_lv_3dtexture_t.safe_field("h_flip", 0))

    @property
    def v_flip(self):
        return int(self._wv_lv_3dtexture_t.safe_field("v_flip", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["id"] = self.id
        d["h_flip"] = self.h_flip
        d["v_flip"] = self.v_flip
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_animimg.py'] = """\"\"\"
Auto-generated wrapper for lv_animimg_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_image import LVImage
from ._helpers import ptr_or_none, safe_wrapper


class LVAnimimg(LVImage):
    \"\"\"LVGL animimage widget (lv_animimg_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_animimg_t = self.cast("lv_animimg_t", ptr=True) or self
        self._wv = self._wv_lv_animimg_t

    @property
    def anim(self):
        return safe_wrapper(self._wv_lv_animimg_t, "anim", "lvglgdb.lvgl.misc.lv_anim", "LVAnim")

    @property
    def dsc(self):
        return ptr_or_none(self._wv_lv_animimg_t.safe_field("dsc"))

    @property
    def pic_count(self):
        return int(self._wv_lv_animimg_t.safe_field("pic_count", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["anim"] = self.anim
        d["dsc"] = self.dsc
        d["pic_count"] = self.pic_count
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_arc.py'] = """\"\"\"
Auto-generated wrapper for lv_arc_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVArc(LVObject):
    \"\"\"LVGL arc widget (lv_arc_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_arc_t = self.cast("lv_arc_t", ptr=True) or self
        self._wv = self._wv_lv_arc_t

    @property
    def rotation(self):
        return int(self._wv_lv_arc_t.safe_field("rotation", 0))

    @property
    def indic_angle_start(self):
        return int(self._wv_lv_arc_t.safe_field("indic_angle_start", 0))

    @property
    def indic_angle_end(self):
        return int(self._wv_lv_arc_t.safe_field("indic_angle_end", 0))

    @property
    def bg_angle_start(self):
        return int(self._wv_lv_arc_t.safe_field("bg_angle_start", 0))

    @property
    def bg_angle_end(self):
        return int(self._wv_lv_arc_t.safe_field("bg_angle_end", 0))

    @property
    def value(self):
        \"\"\"Current value of the arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("value", 0))

    @property
    def min_value(self):
        \"\"\"Minimum value of the arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("min_value", 0))

    @property
    def max_value(self):
        \"\"\"Maximum value of the arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("max_value", 0))

    @property
    def dragging(self):
        return int(self._wv_lv_arc_t.safe_field("dragging", 0))

    @property
    def type(self):
        return int(self._wv_lv_arc_t.safe_field("type", 0))

    @property
    def min_close(self):
        \"\"\"1: the last pressed angle was closer to minimum end\"\"\"
        return int(self._wv_lv_arc_t.safe_field("min_close", 0))

    @property
    def in_out(self):
        \"\"\"1: The click was within the background arc angles. 0: Click outside\"\"\"
        return int(self._wv_lv_arc_t.safe_field("in_out", 0))

    @property
    def chg_rate(self):
        \"\"\"Drag angle rate of change of the arc (degrees/sec)\"\"\"
        return int(self._wv_lv_arc_t.safe_field("chg_rate", 0))

    @property
    def last_tick(self):
        \"\"\"Last dragging event timestamp of the arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("last_tick", 0))

    @property
    def last_angle(self):
        \"\"\"Last dragging angle of the arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("last_angle", 0))

    @property
    def knob_offset(self):
        \"\"\"knob offset from the main arc\"\"\"
        return int(self._wv_lv_arc_t.safe_field("knob_offset", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["rotation"] = self.rotation
        d["indic_angle_start"] = self.indic_angle_start
        d["indic_angle_end"] = self.indic_angle_end
        d["bg_angle_start"] = self.bg_angle_start
        d["bg_angle_end"] = self.bg_angle_end
        d["value"] = self.value
        d["min_value"] = self.min_value
        d["max_value"] = self.max_value
        d["dragging"] = self.dragging
        d["type"] = self.type
        d["min_close"] = self.min_close
        d["in_out"] = self.in_out
        d["chg_rate"] = self.chg_rate
        d["last_tick"] = self.last_tick
        d["last_angle"] = self.last_angle
        d["knob_offset"] = self.knob_offset
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_arclabel.py'] = """\"\"\"
Auto-generated wrapper for lv_arclabel_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_point, safe_string


class LVArclabel(LVObject):
    \"\"\"LVGL arclabel widget (lv_arclabel_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_arclabel_t = self.cast("lv_arclabel_t", ptr=True) or self
        self._wv = self._wv_lv_arclabel_t

    @property
    def text(self):
        return safe_string(self._wv_lv_arclabel_t, "text")

    @property
    def dot_begin(self):
        \"\"\"Offset where bytes have been replaced with dots\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("dot_begin", 0))

    @property
    def angle_start(self):
        return int(self._wv_lv_arclabel_t.safe_field("angle_start", 0))

    @property
    def angle_size(self):
        return int(self._wv_lv_arclabel_t.safe_field("angle_size", 0))

    @property
    def offset(self):
        return int(self._wv_lv_arclabel_t.safe_field("offset", 0))

    @property
    def radius(self):
        return int(self._wv_lv_arclabel_t.safe_field("radius", 0))

    @property
    def center_offset(self):
        return safe_point(self._wv_lv_arclabel_t, "center_offset")

    @property
    def dir(self):
        return int(self._wv_lv_arclabel_t.safe_field("dir", 0))

    @property
    def text_align_v(self):
        \"\"\"Vertical text alignment\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("text_align_v", 0))

    @property
    def text_align_h(self):
        \"\"\"Horizontal text alignment\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("text_align_h", 0))

    @property
    def static_txt(self):
        \"\"\"Flag to indicate the text is static\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("static_txt", 0))

    @property
    def recolor(self):
        \"\"\"Enable in-line letter re-coloring\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("recolor", 0))

    @property
    def overflow(self):
        \"\"\"Overflow mode: 0=visible, 1=ellipsis, 2=clip\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("overflow", 0))

    @property
    def end_overlap(self):
        \"\"\"End overlap flag, false if prevent end overlap\"\"\"
        return int(self._wv_lv_arclabel_t.safe_field("end_overlap", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["text"] = self.text
        d["dot_begin"] = self.dot_begin
        d["angle_start"] = self.angle_start
        d["angle_size"] = self.angle_size
        d["offset"] = self.offset
        d["radius"] = self.radius
        d["center_offset"] = self.center_offset
        d["dir"] = self.dir
        d["text_align_v"] = self.text_align_v
        d["text_align_h"] = self.text_align_h
        d["static_txt"] = self.static_txt
        d["recolor"] = self.recolor
        d["overflow"] = self.overflow
        d["end_overlap"] = self.end_overlap
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_bar.py'] = """\"\"\"
Auto-generated wrapper for lv_bar_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_area


class LVBar(LVObject):
    \"\"\"LVGL bar widget (lv_bar_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_bar_t = self.cast("lv_bar_t", ptr=True) or self
        self._wv = self._wv_lv_bar_t

    @property
    def cur_value(self):
        \"\"\"Current value of the bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("cur_value", 0))

    @property
    def min_value(self):
        \"\"\"Minimum value of the bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("min_value", 0))

    @property
    def max_value(self):
        \"\"\"Maximum value of the bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("max_value", 0))

    @property
    def start_value(self):
        \"\"\"Start value of the bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("start_value", 0))

    @property
    def indic_area(self):
        \"\"\"Save the indicator area. Might be used by derived types\"\"\"
        return safe_area(self._wv_lv_bar_t, "indic_area")

    @property
    def val_reversed(self):
        \"\"\"Whether value been reversed\"\"\"
        return int(self._wv_lv_bar_t.safe_field("val_reversed", 0))

    @property
    def mode(self):
        \"\"\"Type of bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("mode", 0))

    @property
    def orientation(self):
        \"\"\"Orientation of bar\"\"\"
        return int(self._wv_lv_bar_t.safe_field("orientation", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["cur_value"] = self.cur_value
        d["min_value"] = self.min_value
        d["max_value"] = self.max_value
        d["start_value"] = self.start_value
        d["indic_area"] = self.indic_area
        d["val_reversed"] = self.val_reversed
        d["mode"] = self.mode
        d["orientation"] = self.orientation
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_barcode.py'] = """\"\"\"
Auto-generated wrapper for lv_barcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_canvas import LVCanvas
from ._helpers import ptr_or_none, safe_color, safe_string


class LVBarcode(LVCanvas):
    \"\"\"LVGL barcode widget (lv_barcode_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_barcode_t = self.cast("lv_barcode_t", ptr=True) or self
        self._wv = self._wv_lv_barcode_t

    @property
    def dark_color(self):
        return safe_color(self._wv_lv_barcode_t, "dark_color")

    @property
    def light_color(self):
        return safe_color(self._wv_lv_barcode_t, "light_color")

    @property
    def data(self):
        \"\"\"Copy of the payload, kept so the bitmap can be regenerated on a property change\"\"\"
        return safe_string(self._wv_lv_barcode_t, "data")

    @property
    def pattern(self):
        return ptr_or_none(self._wv_lv_barcode_t.safe_field("pattern"))

    @property
    def bar_count(self):
        \"\"\"Bars `data` encodes to; 0 when it is not known and has to be encoded\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("bar_count", 0))

    @property
    def scale(self):
        \"\"\"Pixel width of a single bar\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("scale", 0))

    @property
    def direction(self):
        return int(self._wv_lv_barcode_t.safe_field("direction", 0))

    @property
    def encoding(self):
        return int(self._wv_lv_barcode_t.safe_field("encoding", 0))

    @property
    def tiled(self):
        \"\"\"Draw a one bar wide bitmap and let the image tiling repeat it\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("tiled", 0))

    @property
    def update_mode(self):
        \"\"\"lv_barcode_update_mode_t: when a property change is regenerated\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("update_mode", 0))

    @property
    def needs_update(self):
        \"\"\"The bitmap is out of date; filled in on the next redraw (deferred mode)\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("needs_update", 0))

    @property
    def render_valid(self):
        \"\"\"No generation attempt is known to have failed; a change re-arms it\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("render_valid", 0))

    @property
    def fitting(self):
        \"\"\"Guard against the re-entrant resize our own reallocation triggers\"\"\"
        return int(self._wv_lv_barcode_t.safe_field("fitting", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["data"] = self.data
        d["pattern"] = self.pattern
        d["bar_count"] = self.bar_count
        d["scale"] = self.scale
        d["direction"] = self.direction
        d["encoding"] = self.encoding
        d["tiled"] = self.tiled
        d["update_mode"] = self.update_mode
        d["needs_update"] = self.needs_update
        d["render_valid"] = self.render_valid
        d["fitting"] = self.fitting
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_button.py'] = """\"\"\"
Auto-generated wrapper for lv_button_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVButton(LVObject):
    \"\"\"LVGL button widget (lv_button_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_button_t = self.cast("lv_button_t", ptr=True) or self
        self._wv = self._wv_lv_button_t

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_buttonmatrix.py'] = """\"\"\"
Auto-generated wrapper for lv_buttonmatrix_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVButtonmatrix(LVObject):
    \"\"\"LVGL buttonmatrix widget (lv_buttonmatrix_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_buttonmatrix_t = self.cast("lv_buttonmatrix_t", ptr=True) or self
        self._wv = self._wv_lv_buttonmatrix_t

    @property
    def map_p(self):
        \"\"\"Pointer to the current map\"\"\"
        return ptr_or_none(self._wv_lv_buttonmatrix_t.safe_field("map_p"))

    @property
    def button_areas(self):
        \"\"\"Array of areas of buttons\"\"\"
        return ptr_or_none(self._wv_lv_buttonmatrix_t.safe_field("button_areas"))

    @property
    def ctrl_bits(self):
        \"\"\"Array of control bytes\"\"\"
        return ptr_or_none(self._wv_lv_buttonmatrix_t.safe_field("ctrl_bits"))

    @property
    def btn_cnt(self):
        \"\"\"Number of button in 'map_p'(Handled by the library)\"\"\"
        return int(self._wv_lv_buttonmatrix_t.safe_field("btn_cnt", 0))

    @property
    def row_cnt(self):
        \"\"\"Number of rows in 'map_p'(Handled by the library)\"\"\"
        return int(self._wv_lv_buttonmatrix_t.safe_field("row_cnt", 0))

    @property
    def btn_id_sel(self):
        \"\"\"Index of the active button (being pressed/released etc) or LV_BUTTONMATRIX_BUTTON_NONE\"\"\"
        return int(self._wv_lv_buttonmatrix_t.safe_field("btn_id_sel", 0))

    @property
    def one_check(self):
        \"\"\"1: Single button toggled at once\"\"\"
        return int(self._wv_lv_buttonmatrix_t.safe_field("one_check", 0))

    @property
    def auto_free_map(self):
        \"\"\"1: Automatically free the map when the widget is deleted\"\"\"
        return int(self._wv_lv_buttonmatrix_t.safe_field("auto_free_map", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["map_p"] = self.map_p
        d["button_areas"] = self.button_areas
        d["ctrl_bits"] = self.ctrl_bits
        d["btn_cnt"] = self.btn_cnt
        d["row_cnt"] = self.row_cnt
        d["btn_id_sel"] = self.btn_id_sel
        d["one_check"] = self.one_check
        d["auto_free_map"] = self.auto_free_map
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_calendar.py'] = """\"\"\"
Auto-generated wrapper for lv_calendar_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVCalendar(LVObject):
    \"\"\"LVGL calendar widget (lv_calendar_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_calendar_t = self.cast("lv_calendar_t", ptr=True) or self
        self._wv = self._wv_lv_calendar_t

    @property
    def btnm(self):
        return ptr_or_none(self._wv_lv_calendar_t.safe_field("btnm"))

    @property
    def highlighted_dates(self):
        \"\"\"Apply different style on these days (pointer to user-defined array)\"\"\"
        return ptr_or_none(self._wv_lv_calendar_t.safe_field("highlighted_dates"))

    @property
    def highlighted_dates_num(self):
        \"\"\"Number of elements in `highlighted_days`\"\"\"
        return int(self._wv_lv_calendar_t.safe_field("highlighted_dates_num", 0))

    @property
    def use_chinese_calendar(self):
        return int(self._wv_lv_calendar_t.safe_field("use_chinese_calendar", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["btnm"] = self.btnm
        d["highlighted_dates"] = self.highlighted_dates
        d["highlighted_dates_num"] = self.highlighted_dates_num
        d["use_chinese_calendar"] = self.use_chinese_calendar
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_canvas.py'] = """\"\"\"
Auto-generated wrapper for lv_canvas_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_image import LVImage
from ._helpers import ptr_or_none, safe_wrapper


class LVCanvas(LVImage):
    \"\"\"LVGL canvas widget (lv_canvas_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_canvas_t = self.cast("lv_canvas_t", ptr=True) or self
        self._wv = self._wv_lv_canvas_t

    @property
    def draw_buf(self):
        return ptr_or_none(self._wv_lv_canvas_t.safe_field("draw_buf"))

    @property
    def static_buf(self):
        return safe_wrapper(self._wv_lv_canvas_t, "static_buf", "lvglgdb.lvgl.draw.lv_draw_buf", "LVDrawBuf")

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["draw_buf"] = self.draw_buf
        d["static_buf"] = self.static_buf
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_chart.py'] = """\"\"\"
Auto-generated wrapper for lv_chart_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_wrapper


class LVChart(LVObject):
    \"\"\"LVGL chart widget (lv_chart_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_chart_t = self.cast("lv_chart_t", ptr=True) or self
        self._wv = self._wv_lv_chart_t

    @property
    def series_ll(self):
        \"\"\"Linked list for series (stores lv_chart_series_t)\"\"\"
        return safe_wrapper(self._wv_lv_chart_t, "series_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cursor_ll(self):
        \"\"\"Linked list for cursors (stores lv_chart_cursor_t)\"\"\"
        return safe_wrapper(self._wv_lv_chart_t, "cursor_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def pressed_point_id(self):
        return int(self._wv_lv_chart_t.safe_field("pressed_point_id", 0))

    @property
    def hdiv_cnt(self):
        \"\"\"Number of horizontal division lines\"\"\"
        return int(self._wv_lv_chart_t.safe_field("hdiv_cnt", 0))

    @property
    def vdiv_cnt(self):
        \"\"\"Number of vertical division lines\"\"\"
        return int(self._wv_lv_chart_t.safe_field("vdiv_cnt", 0))

    @property
    def point_cnt(self):
        \"\"\"Number of points in all series\"\"\"
        return int(self._wv_lv_chart_t.safe_field("point_cnt", 0))

    @property
    def type(self):
        \"\"\"Chart type\"\"\"
        return int(self._wv_lv_chart_t.safe_field("type", 0))

    @property
    def update_mode(self):
        return int(self._wv_lv_chart_t.safe_field("update_mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["series_ll"] = self.series_ll
        d["cursor_ll"] = self.cursor_ll
        d["pressed_point_id"] = self.pressed_point_id
        d["hdiv_cnt"] = self.hdiv_cnt
        d["vdiv_cnt"] = self.vdiv_cnt
        d["point_cnt"] = self.point_cnt
        d["type"] = self.type
        d["update_mode"] = self.update_mode
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_checkbox.py'] = """\"\"\"
Auto-generated wrapper for lv_checkbox_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_string


class LVCheckbox(LVObject):
    \"\"\"LVGL checkbox widget (lv_checkbox_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_checkbox_t = self.cast("lv_checkbox_t", ptr=True) or self
        self._wv = self._wv_lv_checkbox_t

    @property
    def txt(self):
        return safe_string(self._wv_lv_checkbox_t, "txt")

    @property
    def static_txt(self):
        return int(self._wv_lv_checkbox_t.safe_field("static_txt", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["txt"] = self.txt
        d["static_txt"] = self.static_txt
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_dropdown.py'] = """\"\"\"
Auto-generated wrapper for lv_dropdown_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_string


class LVDropdown(LVObject):
    \"\"\"LVGL dropdown widget (lv_dropdown_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_dropdown_t = self.cast("lv_dropdown_t", ptr=True) or self
        self._wv = self._wv_lv_dropdown_t

    @property
    def list(self):
        \"\"\"The dropped down list\"\"\"
        return ptr_or_none(self._wv_lv_dropdown_t.safe_field("list"))

    @property
    def text(self):
        \"\"\"Text to display on the dropdown's button\"\"\"
        return safe_string(self._wv_lv_dropdown_t, "text")

    @property
    def symbol(self):
        \"\"\"Arrow or other icon when the drop-down list is closed\"\"\"
        return ptr_or_none(self._wv_lv_dropdown_t.safe_field("symbol"))

    @property
    def options(self):
        \"\"\"Options in a '\\n' separated list\"\"\"
        return safe_string(self._wv_lv_dropdown_t, "options")

    @property
    def text_translation_tag(self):
        \"\"\"Translation tag for the button's text\"\"\"
        return safe_string(self._wv_lv_dropdown_t, "text_translation_tag")

    @property
    def options_translation_tag(self):
        \"\"\"Translation tag for the options\"\"\"
        return safe_string(self._wv_lv_dropdown_t, "options_translation_tag")

    @property
    def option_cnt(self):
        \"\"\"Number of options\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("option_cnt", 0))

    @property
    def sel_opt_id(self):
        \"\"\"Index of the currently selected option\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("sel_opt_id", 0))

    @property
    def sel_opt_id_orig(self):
        \"\"\"Store the original index on focus\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("sel_opt_id_orig", 0))

    @property
    def pr_opt_id(self):
        \"\"\"Index of the currently pressed option\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("pr_opt_id", 0))

    @property
    def dir(self):
        \"\"\"Direction in which the list should open\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("dir", 0))

    @property
    def static_options(self):
        \"\"\"1: Only a pointer is saved in `options`\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("static_options", 0))

    @property
    def selected_highlight(self):
        \"\"\"1: Make the selected option highlighted in the list\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("selected_highlight", 0))

    @property
    def static_text(self):
        \"\"\"1: Only a pointer is saved in `text`\"\"\"
        return int(self._wv_lv_dropdown_t.safe_field("static_text", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["list"] = self.list
        d["text"] = self.text
        d["symbol"] = self.symbol
        d["options"] = self.options
        d["text_translation_tag"] = self.text_translation_tag
        d["options_translation_tag"] = self.options_translation_tag
        d["option_cnt"] = self.option_cnt
        d["sel_opt_id"] = self.sel_opt_id
        d["sel_opt_id_orig"] = self.sel_opt_id_orig
        d["pr_opt_id"] = self.pr_opt_id
        d["dir"] = self.dir
        d["static_options"] = self.static_options
        d["selected_highlight"] = self.selected_highlight
        d["static_text"] = self.static_text
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_dropdown_list.py'] = """\"\"\"
Auto-generated wrapper for lv_dropdown_list_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVDropdownList(LVObject):
    \"\"\"LVGL dropdown widget (lv_dropdown_list_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_dropdown_list_t = self.cast("lv_dropdown_list_t", ptr=True) or self
        self._wv = self._wv_lv_dropdown_list_t

    @property
    def dropdown(self):
        return ptr_or_none(self._wv_lv_dropdown_list_t.safe_field("dropdown"))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dropdown"] = self.dropdown
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_ffmpeg_player.py'] = """\"\"\"
Auto-generated wrapper for lv_ffmpeg_player_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_image import LVImage
from ._helpers import ptr_or_none, safe_string


class LVFfmpegPlayer(LVImage):
    \"\"\"LVGL ffmpeg widget (lv_ffmpeg_player_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_ffmpeg_player_t = self.cast("lv_ffmpeg_player_t", ptr=True) or self
        self._wv = self._wv_lv_ffmpeg_player_t

    @property
    def timer(self):
        return ptr_or_none(self._wv_lv_ffmpeg_player_t.safe_field("timer"))

    @property
    def auto_restart(self):
        return int(self._wv_lv_ffmpeg_player_t.safe_field("auto_restart", 0))

    @property
    def decoder_name(self):
        return safe_string(self._wv_lv_ffmpeg_player_t, "decoder_name")

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["timer"] = self.timer
        d["auto_restart"] = self.auto_restart
        d["decoder_name"] = self.decoder_name
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_gstreamer.py'] = """\"\"\"
Auto-generated wrapper for lv_gstreamer_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_image import LVImage
from ._helpers import ptr_or_none


class LVGstreamer(LVImage):
    \"\"\"LVGL gstreamer widget (lv_gstreamer_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_gstreamer_t = self.cast("lv_gstreamer_t", ptr=True) or self
        self._wv = self._wv_lv_gstreamer_t

    @property
    def last_buffer(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("last_buffer"))

    @property
    def last_sample(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("last_sample"))

    @property
    def pipeline(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("pipeline"))

    @property
    def audio_convert(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("audio_convert"))

    @property
    def video_convert(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("video_convert"))

    @property
    def audio_volume(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("audio_volume"))

    @property
    def aligned_frame(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("aligned_frame"))

    @property
    def color_format(self):
        return int(self._wv_lv_gstreamer_t.safe_field("color_format", 0))

    @property
    def gstreamer_timer(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("gstreamer_timer"))

    @property
    def frame_queue(self):
        return ptr_or_none(self._wv_lv_gstreamer_t.safe_field("frame_queue"))

    @property
    def is_video_info_valid(self):
        return int(self._wv_lv_gstreamer_t.safe_field("is_video_info_valid", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["last_buffer"] = self.last_buffer
        d["last_sample"] = self.last_sample
        d["pipeline"] = self.pipeline
        d["audio_convert"] = self.audio_convert
        d["video_convert"] = self.video_convert
        d["audio_volume"] = self.audio_volume
        d["aligned_frame"] = self.aligned_frame
        d["color_format"] = self.color_format
        d["gstreamer_timer"] = self.gstreamer_timer
        d["frame_queue"] = self.frame_queue
        d["is_video_info_valid"] = self.is_video_info_valid
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_image.py'] = """\"\"\"
Auto-generated wrapper for lv_image_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_point


class LVImage(LVObject):
    \"\"\"LVGL image widget (lv_image_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_image_t = self.cast("lv_image_t", ptr=True) or self
        self._wv = self._wv_lv_image_t

    @property
    def src(self):
        \"\"\"Image source: Pointer to an array or a file or a symbol\"\"\"
        return ptr_or_none(self._wv_lv_image_t.safe_field("src"))

    @property
    def bitmap_mask_src(self):
        \"\"\"Pointer to an A8 bitmap mask\"\"\"
        return ptr_or_none(self._wv_lv_image_t.safe_field("bitmap_mask_src"))

    @property
    def offset(self):
        return safe_point(self._wv_lv_image_t, "offset")

    @property
    def w(self):
        \"\"\"Width of the image (Handled by the library)\"\"\"
        return int(self._wv_lv_image_t.safe_field("w", 0))

    @property
    def h(self):
        \"\"\"Height of the image (Handled by the library)\"\"\"
        return int(self._wv_lv_image_t.safe_field("h", 0))

    @property
    def rotation(self):
        \"\"\"Rotation angle of the image\"\"\"
        return int(self._wv_lv_image_t.safe_field("rotation", 0))

    @property
    def scale_x(self):
        \"\"\"256 means no zoom, 512 double size, 128 half size\"\"\"
        return int(self._wv_lv_image_t.safe_field("scale_x", 0))

    @property
    def scale_y(self):
        \"\"\"256 means no zoom, 512 double size, 128 half size\"\"\"
        return int(self._wv_lv_image_t.safe_field("scale_y", 0))

    @property
    def pivot(self):
        \"\"\"Rotation center of the image\"\"\"
        return safe_point(self._wv_lv_image_t, "pivot")

    @property
    def src_type(self):
        \"\"\"See: lv_image_src_t\"\"\"
        return int(self._wv_lv_image_t.safe_field("src_type", 0))

    @property
    def cf(self):
        \"\"\"Color format from `lv_color_format_t`\"\"\"
        return int(self._wv_lv_image_t.safe_field("cf", 0))

    @property
    def antialias(self):
        \"\"\"Apply anti-aliasing in transformations (rotate, zoom)\"\"\"
        return int(self._wv_lv_image_t.safe_field("antialias", 0))

    @property
    def align(self):
        \"\"\"Image size mode when image size and object size is different. See lv_image_align_t\"\"\"
        return int(self._wv_lv_image_t.safe_field("align", 0))

    @property
    def blend_mode(self):
        \"\"\"Element of `lv_blend_mode_t`\"\"\"
        return int(self._wv_lv_image_t.safe_field("blend_mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["src"] = self.src
        d["bitmap_mask_src"] = self.bitmap_mask_src
        d["offset"] = self.offset
        d["w"] = self.w
        d["h"] = self.h
        d["rotation"] = self.rotation
        d["scale_x"] = self.scale_x
        d["scale_y"] = self.scale_y
        d["pivot"] = self.pivot
        d["src_type"] = self.src_type
        d["cf"] = self.cf
        d["antialias"] = self.antialias
        d["align"] = self.align
        d["blend_mode"] = self.blend_mode
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_imagebutton.py'] = """\"\"\"
Auto-generated wrapper for lv_imagebutton_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVImagebutton(LVObject):
    \"\"\"LVGL imagebutton widget (lv_imagebutton_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_imagebutton_t = self.cast("lv_imagebutton_t", ptr=True) or self
        self._wv = self._wv_lv_imagebutton_t

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_ime_pinyin.py'] = """\"\"\"
Auto-generated wrapper for lv_ime_pinyin_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_string, safe_wrapper


class LVImePinyin(LVObject):
    \"\"\"LVGL ime widget (lv_ime_pinyin_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_ime_pinyin_t = self.cast("lv_ime_pinyin_t", ptr=True) or self
        self._wv = self._wv_lv_ime_pinyin_t

    @property
    def kb(self):
        return ptr_or_none(self._wv_lv_ime_pinyin_t.safe_field("kb"))

    @property
    def cand_panel(self):
        return ptr_or_none(self._wv_lv_ime_pinyin_t.safe_field("cand_panel"))

    @property
    def dict(self):
        return ptr_or_none(self._wv_lv_ime_pinyin_t.safe_field("dict"))

    @property
    def k9_legal_py_ll(self):
        return safe_wrapper(self._wv_lv_ime_pinyin_t, "k9_legal_py_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cand_str(self):
        \"\"\"Candidate string\"\"\"
        return safe_string(self._wv_lv_ime_pinyin_t, "cand_str")

    @property
    def k9_py_ll_pos(self):
        \"\"\"Current pinyin map pages(k9)\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("k9_py_ll_pos", 0))

    @property
    def k9_legal_py_count(self):
        \"\"\"Count of legal Pinyin numbers(k9)\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("k9_legal_py_count", 0))

    @property
    def k9_input_str_len(self):
        \"\"\"9-key input(k9) mode input string max len\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("k9_input_str_len", 0))

    @property
    def ta_count(self):
        \"\"\"The number of characters entered in the text box this time\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("ta_count", 0))

    @property
    def cand_num(self):
        \"\"\"Number of candidates\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("cand_num", 0))

    @property
    def py_page(self):
        \"\"\"Current pinyin map pages(k26)\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("py_page", 0))

    @property
    def mode(self):
        \"\"\"Set mode, 1: 26-key input(k26), 0: 9-key input(k9). Default: 1.\"\"\"
        return int(self._wv_lv_ime_pinyin_t.safe_field("mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["kb"] = self.kb
        d["cand_panel"] = self.cand_panel
        d["dict"] = self.dict
        d["k9_legal_py_ll"] = self.k9_legal_py_ll
        d["cand_str"] = self.cand_str
        d["k9_py_ll_pos"] = self.k9_py_ll_pos
        d["k9_legal_py_count"] = self.k9_legal_py_count
        d["k9_input_str_len"] = self.k9_input_str_len
        d["ta_count"] = self.ta_count
        d["cand_num"] = self.cand_num
        d["py_page"] = self.py_page
        d["mode"] = self.mode
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_keyboard.py'] = """\"\"\"
Auto-generated wrapper for lv_keyboard_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_buttonmatrix import LVButtonmatrix
from ._helpers import ptr_or_none


class LVKeyboard(LVButtonmatrix):
    \"\"\"LVGL keyboard widget (lv_keyboard_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_keyboard_t = self.cast("lv_keyboard_t", ptr=True) or self
        self._wv = self._wv_lv_keyboard_t

    @property
    def ta(self):
        \"\"\"Pointer to the assigned text area\"\"\"
        return ptr_or_none(self._wv_lv_keyboard_t.safe_field("ta"))

    @property
    def mode(self):
        \"\"\"Key map type\"\"\"
        return int(self._wv_lv_keyboard_t.safe_field("mode", 0))

    @property
    def popovers(self):
        \"\"\"Show button titles in popovers on press\"\"\"
        return int(self._wv_lv_keyboard_t.safe_field("popovers", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["ta"] = self.ta
        d["mode"] = self.mode
        d["popovers"] = self.popovers
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_label.py'] = """\"\"\"
Auto-generated wrapper for lv_label_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_point, safe_string


class LVLabel(LVObject):
    \"\"\"LVGL label widget (lv_label_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_label_t = self.cast("lv_label_t", ptr=True) or self
        self._wv = self._wv_lv_label_t

    @property
    def text(self):
        return safe_string(self._wv_lv_label_t, "text")

    @property
    def translation_tag(self):
        return safe_string(self._wv_lv_label_t, "translation_tag")

    @property
    def dot_begin(self):
        \"\"\"Offset where bytes have been replaced with dots\"\"\"
        return int(self._wv_lv_label_t.safe_field("dot_begin", 0))

    @property
    def max_lines(self):
        return int(self._wv_lv_label_t.safe_field("max_lines", 0))

    @property
    def sel_start(self):
        return int(self._wv_lv_label_t.safe_field("sel_start", 0))

    @property
    def sel_end(self):
        return int(self._wv_lv_label_t.safe_field("sel_end", 0))

    @property
    def size_cache(self):
        \"\"\"Text size cache\"\"\"
        return safe_point(self._wv_lv_label_t, "size_cache")

    @property
    def offset(self):
        \"\"\"Text draw position offset\"\"\"
        return safe_point(self._wv_lv_label_t, "offset")

    @property
    def long_mode(self):
        \"\"\"Determine what to do with the long texts\"\"\"
        return int(self._wv_lv_label_t.safe_field("long_mode", 0))

    @property
    def static_txt(self):
        \"\"\"Flag to indicate the text is static\"\"\"
        return int(self._wv_lv_label_t.safe_field("static_txt", 0))

    @property
    def recolor(self):
        \"\"\"Enable in-line letter re-coloring\"\"\"
        return int(self._wv_lv_label_t.safe_field("recolor", 0))

    @property
    def expand(self):
        \"\"\"Ignore real width (used by the library with LV_LABEL_LONG_MODE_SCROLL)\"\"\"
        return int(self._wv_lv_label_t.safe_field("expand", 0))

    @property
    def invalid_size_cache(self):
        \"\"\"1: Recalculate size and update cache\"\"\"
        return int(self._wv_lv_label_t.safe_field("invalid_size_cache", 0))

    @property
    def need_refr_text(self):
        \"\"\"1: Refresh text after layout update completion\"\"\"
        return int(self._wv_lv_label_t.safe_field("need_refr_text", 0))

    @property
    def text_size(self):
        return safe_point(self._wv_lv_label_t, "text_size")

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["text"] = self.text
        d["translation_tag"] = self.translation_tag
        d["dot_begin"] = self.dot_begin
        d["max_lines"] = self.max_lines
        d["sel_start"] = self.sel_start
        d["sel_end"] = self.sel_end
        d["size_cache"] = self.size_cache
        d["offset"] = self.offset
        d["long_mode"] = self.long_mode
        d["static_txt"] = self.static_txt
        d["recolor"] = self.recolor
        d["expand"] = self.expand
        d["invalid_size_cache"] = self.invalid_size_cache
        d["need_refr_text"] = self.need_refr_text
        d["text_size"] = self.text_size
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_led.py'] = """\"\"\"
Auto-generated wrapper for lv_led_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_color


class LVLed(LVObject):
    \"\"\"LVGL led widget (lv_led_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_led_t = self.cast("lv_led_t", ptr=True) or self
        self._wv = self._wv_lv_led_t

    @property
    def color(self):
        return safe_color(self._wv_lv_led_t, "color")

    @property
    def bright(self):
        \"\"\"Current brightness of the LED (0..255)\"\"\"
        return int(self._wv_lv_led_t.safe_field("bright", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["color"] = self.color
        d["bright"] = self.bright
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_line.py'] = """\"\"\"
Auto-generated wrapper for lv_line_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVLine(LVObject):
    \"\"\"LVGL line widget (lv_line_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_line_t = self.cast("lv_line_t", ptr=True) or self
        self._wv = self._wv_lv_line_t

    @property
    def point_num(self):
        \"\"\"Number of points in 'point_array'\"\"\"
        return int(self._wv_lv_line_t.safe_field("point_num", 0))

    @property
    def y_inv(self):
        \"\"\"1: y == 0 will be on the bottom\"\"\"
        return int(self._wv_lv_line_t.safe_field("y_inv", 0))

    @property
    def point_array_is_mutable(self):
        \"\"\"whether the point array is const or mutable\"\"\"
        return int(self._wv_lv_line_t.safe_field("point_array_is_mutable", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["point_num"] = self.point_num
        d["y_inv"] = self.y_inv
        d["point_array_is_mutable"] = self.point_array_is_mutable
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_menu.py'] = """\"\"\"
Auto-generated wrapper for lv_menu_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_wrapper


class LVMenu(LVObject):
    \"\"\"LVGL menu widget (lv_menu_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_menu_t = self.cast("lv_menu_t", ptr=True) or self
        self._wv = self._wv_lv_menu_t

    @property
    def storage(self):
        \"\"\"a pointer to obj that is the parent of all pages not displayed\"\"\"
        return ptr_or_none(self._wv_lv_menu_t.safe_field("storage"))

    @property
    def main(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("main"))

    @property
    def main_page(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("main_page"))

    @property
    def main_header(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("main_header"))

    @property
    def main_header_title(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("main_header_title"))

    @property
    def sidebar(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("sidebar"))

    @property
    def sidebar_page(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("sidebar_page"))

    @property
    def sidebar_header(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("sidebar_header"))

    @property
    def sidebar_header_title(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("sidebar_header_title"))

    @property
    def selected_tab(self):
        return ptr_or_none(self._wv_lv_menu_t.safe_field("selected_tab"))

    @property
    def history_ll(self):
        return safe_wrapper(self._wv_lv_menu_t, "history_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cur_depth(self):
        return int(self._wv_lv_menu_t.safe_field("cur_depth", 0))

    @property
    def prev_depth(self):
        return int(self._wv_lv_menu_t.safe_field("prev_depth", 0))

    @property
    def sidebar_generated(self):
        return int(self._wv_lv_menu_t.safe_field("sidebar_generated", 0))

    @property
    def mode_header(self):
        return int(self._wv_lv_menu_t.safe_field("mode_header", 0))

    @property
    def mode_root_back_btn(self):
        return int(self._wv_lv_menu_t.safe_field("mode_root_back_btn", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["storage"] = self.storage
        d["main"] = self.main
        d["main_page"] = self.main_page
        d["main_header"] = self.main_header
        d["main_header_title"] = self.main_header_title
        d["sidebar"] = self.sidebar
        d["sidebar_page"] = self.sidebar_page
        d["sidebar_header"] = self.sidebar_header
        d["sidebar_header_title"] = self.sidebar_header_title
        d["selected_tab"] = self.selected_tab
        d["history_ll"] = self.history_ll
        d["cur_depth"] = self.cur_depth
        d["prev_depth"] = self.prev_depth
        d["sidebar_generated"] = self.sidebar_generated
        d["mode_header"] = self.mode_header
        d["mode_root_back_btn"] = self.mode_root_back_btn
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_menu_page.py'] = """\"\"\"
Auto-generated wrapper for lv_menu_page_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_string


class LVMenuPage(LVObject):
    \"\"\"LVGL menu widget (lv_menu_page_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_menu_page_t = self.cast("lv_menu_page_t", ptr=True) or self
        self._wv = self._wv_lv_menu_page_t

    @property
    def title(self):
        return safe_string(self._wv_lv_menu_page_t, "title")

    @property
    def static_title(self):
        return int(self._wv_lv_menu_page_t.safe_field("static_title", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["title"] = self.title
        d["static_title"] = self.static_title
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_msgbox.py'] = """\"\"\"
Auto-generated wrapper for lv_msgbox_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVMsgbox(LVObject):
    \"\"\"LVGL msgbox widget (lv_msgbox_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_msgbox_t = self.cast("lv_msgbox_t", ptr=True) or self
        self._wv = self._wv_lv_msgbox_t

    @property
    def header(self):
        return ptr_or_none(self._wv_lv_msgbox_t.safe_field("header"))

    @property
    def content(self):
        return ptr_or_none(self._wv_lv_msgbox_t.safe_field("content"))

    @property
    def footer(self):
        return ptr_or_none(self._wv_lv_msgbox_t.safe_field("footer"))

    @property
    def title(self):
        return ptr_or_none(self._wv_lv_msgbox_t.safe_field("title"))

    @property
    def auto_parent(self):
        \"\"\"Mark that the parent was automatically created\"\"\"
        return int(self._wv_lv_msgbox_t.safe_field("auto_parent", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["header"] = self.header
        d["content"] = self.content
        d["footer"] = self.footer
        d["title"] = self.title
        d["auto_parent"] = self.auto_parent
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_qrcode.py'] = """\"\"\"
Auto-generated wrapper for lv_qrcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_canvas import LVCanvas
from ._helpers import ptr_or_none, safe_color


class LVQrcode(LVCanvas):
    \"\"\"LVGL qrcode widget (lv_qrcode_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_qrcode_t = self.cast("lv_qrcode_t", ptr=True) or self
        self._wv = self._wv_lv_qrcode_t

    @property
    def dark_color(self):
        return safe_color(self._wv_lv_qrcode_t, "dark_color")

    @property
    def light_color(self):
        return safe_color(self._wv_lv_qrcode_t, "light_color")

    @property
    def data(self):
        \"\"\"Copy of the payload, kept so the bitmap can be re-encoded on a property change\"\"\"
        return ptr_or_none(self._wv_lv_qrcode_t.safe_field("data"))

    @property
    def data_len(self):
        \"\"\"Stored payload length in bytes\"\"\"
        return int(self._wv_lv_qrcode_t.safe_field("data_len", 0))

    @property
    def quiet_zone(self):
        \"\"\"Add the QR spec's blank margin around the code (boolean toggle)\"\"\"
        return int(self._wv_lv_qrcode_t.safe_field("quiet_zone", 0))

    @property
    def update_mode(self):
        \"\"\"lv_qrcode_update_mode_t: when a property change is re-encoded\"\"\"
        return int(self._wv_lv_qrcode_t.safe_field("update_mode", 0))

    @property
    def needs_update(self):
        \"\"\"The bitmap is out of date; re-encoded on the next redraw (deferred mode)\"\"\"
        return int(self._wv_lv_qrcode_t.safe_field("needs_update", 0))

    @property
    def render_valid(self):
        \"\"\"No encode attempt is known to have failed; a change re-arms it\"\"\"
        return int(self._wv_lv_qrcode_t.safe_field("render_valid", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["data"] = self.data
        d["data_len"] = self.data_len
        d["quiet_zone"] = self.quiet_zone
        d["update_mode"] = self.update_mode
        d["needs_update"] = self.needs_update
        d["render_valid"] = self.render_valid
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_roller.py'] = """\"\"\"
Auto-generated wrapper for lv_roller_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_string


class LVRoller(LVObject):
    \"\"\"LVGL roller widget (lv_roller_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_roller_t = self.cast("lv_roller_t", ptr=True) or self
        self._wv = self._wv_lv_roller_t

    @property
    def options_translation_tag(self):
        \"\"\"Translation tag for the options\"\"\"
        return safe_string(self._wv_lv_roller_t, "options_translation_tag")

    @property
    def option_cnt(self):
        \"\"\"Number of options\"\"\"
        return int(self._wv_lv_roller_t.safe_field("option_cnt", 0))

    @property
    def sel_opt_id(self):
        \"\"\"Index of the current option\"\"\"
        return int(self._wv_lv_roller_t.safe_field("sel_opt_id", 0))

    @property
    def sel_opt_id_ori(self):
        \"\"\"Store the original index on focus\"\"\"
        return int(self._wv_lv_roller_t.safe_field("sel_opt_id_ori", 0))

    @property
    def inf_page_cnt(self):
        \"\"\"Number of extra pages added to make the roller look infinite\"\"\"
        return int(self._wv_lv_roller_t.safe_field("inf_page_cnt", 0))

    @property
    def mode(self):
        return int(self._wv_lv_roller_t.safe_field("mode", 0))

    @property
    def moved(self):
        return int(self._wv_lv_roller_t.safe_field("moved", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["options_translation_tag"] = self.options_translation_tag
        d["option_cnt"] = self.option_cnt
        d["sel_opt_id"] = self.sel_opt_id
        d["sel_opt_id_ori"] = self.sel_opt_id_ori
        d["inf_page_cnt"] = self.inf_page_cnt
        d["mode"] = self.mode
        d["moved"] = self.moved
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_scale.py'] = """\"\"\"
Auto-generated wrapper for lv_scale_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_wrapper


class LVScale(LVObject):
    \"\"\"LVGL scale widget (lv_scale_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_scale_t = self.cast("lv_scale_t", ptr=True) or self
        self._wv = self._wv_lv_scale_t

    @property
    def section_ll(self):
        \"\"\"Linked list for the sections (stores lv_scale_section_t)\"\"\"
        return safe_wrapper(self._wv_lv_scale_t, "section_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def txt_src(self):
        return ptr_or_none(self._wv_lv_scale_t.safe_field("txt_src"))

    @property
    def mode(self):
        \"\"\"Orientation and layout of scale.\"\"\"
        return int(self._wv_lv_scale_t.safe_field("mode", 0))

    @property
    def range_min(self):
        \"\"\"Scale's minimum value\"\"\"
        return int(self._wv_lv_scale_t.safe_field("range_min", 0))

    @property
    def range_max(self):
        \"\"\"Scale's maximum value\"\"\"
        return int(self._wv_lv_scale_t.safe_field("range_max", 0))

    @property
    def total_tick_count(self):
        \"\"\"Total number of ticks (major and minor)\"\"\"
        return int(self._wv_lv_scale_t.safe_field("total_tick_count", 0))

    @property
    def major_tick_every(self):
        \"\"\"Frequency of major ticks to minor ticks\"\"\"
        return int(self._wv_lv_scale_t.safe_field("major_tick_every", 0))

    @property
    def label_enabled(self):
        \"\"\"Draw labels for major ticks?\"\"\"
        return int(self._wv_lv_scale_t.safe_field("label_enabled", 0))

    @property
    def post_draw(self):
        return int(self._wv_lv_scale_t.safe_field("post_draw", 0))

    @property
    def draw_ticks_on_top(self):
        \"\"\"Draw ticks on top of main line?\"\"\"
        return int(self._wv_lv_scale_t.safe_field("draw_ticks_on_top", 0))

    @property
    def angle_range(self):
        \"\"\"Degrees between low end and high end of scale\"\"\"
        return int(self._wv_lv_scale_t.safe_field("angle_range", 0))

    @property
    def rotation(self):
        \"\"\"Clockwise angular offset from 3-o'clock position of low end of scale\"\"\"
        return int(self._wv_lv_scale_t.safe_field("rotation", 0))

    @property
    def custom_label_cnt(self):
        \"\"\"Number of custom labels provided in `txt_src`\"\"\"
        return int(self._wv_lv_scale_t.safe_field("custom_label_cnt", 0))

    @property
    def last_tick_width(self):
        \"\"\"Width of last tick in pixels\"\"\"
        return int(self._wv_lv_scale_t.safe_field("last_tick_width", 0))

    @property
    def first_tick_width(self):
        \"\"\"Width of first tick in pixels\"\"\"
        return int(self._wv_lv_scale_t.safe_field("first_tick_width", 0))

    @property
    def needles(self):
        \"\"\"Needle list of this scale\"\"\"
        return safe_wrapper(self._wv_lv_scale_t, "needles", "lvglgdb.lvgl.misc.lv_array", "LVArray")

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["section_ll"] = self.section_ll
        d["txt_src"] = self.txt_src
        d["mode"] = self.mode
        d["range_min"] = self.range_min
        d["range_max"] = self.range_max
        d["total_tick_count"] = self.total_tick_count
        d["major_tick_every"] = self.major_tick_every
        d["label_enabled"] = self.label_enabled
        d["post_draw"] = self.post_draw
        d["draw_ticks_on_top"] = self.draw_ticks_on_top
        d["angle_range"] = self.angle_range
        d["rotation"] = self.rotation
        d["custom_label_cnt"] = self.custom_label_cnt
        d["last_tick_width"] = self.last_tick_width
        d["first_tick_width"] = self.first_tick_width
        d["needles"] = self.needles
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_slider.py'] = """\"\"\"
Auto-generated wrapper for lv_slider_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_bar import LVBar
from ._helpers import ptr_or_none, safe_area, safe_point


class LVSlider(LVBar):
    \"\"\"LVGL slider widget (lv_slider_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_slider_t = self.cast("lv_slider_t", ptr=True) or self
        self._wv = self._wv_lv_slider_t

    @property
    def left_knob_area(self):
        return safe_area(self._wv_lv_slider_t, "left_knob_area")

    @property
    def right_knob_area(self):
        return safe_area(self._wv_lv_slider_t, "right_knob_area")

    @property
    def pressed_point(self):
        return safe_point(self._wv_lv_slider_t, "pressed_point")

    @property
    def value_to_set(self):
        \"\"\"Which bar value to set\"\"\"
        return ptr_or_none(self._wv_lv_slider_t.safe_field("value_to_set"))

    @property
    def dragging(self):
        \"\"\"1: the slider is being dragged\"\"\"
        return int(self._wv_lv_slider_t.safe_field("dragging", 0))

    @property
    def left_knob_focus(self):
        \"\"\"1: with encoder now the right knob can be adjusted\"\"\"
        return int(self._wv_lv_slider_t.safe_field("left_knob_focus", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["left_knob_area"] = self.left_knob_area
        d["right_knob_area"] = self.right_knob_area
        d["pressed_point"] = self.pressed_point
        d["value_to_set"] = self.value_to_set
        d["dragging"] = self.dragging
        d["left_knob_focus"] = self.left_knob_focus
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_spangroup.py'] = """\"\"\"
Auto-generated wrapper for lv_spangroup_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_wrapper


class LVSpangroup(LVObject):
    \"\"\"LVGL span widget (lv_spangroup_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_spangroup_t = self.cast("lv_spangroup_t", ptr=True) or self
        self._wv = self._wv_lv_spangroup_t

    @property
    def lines(self):
        return int(self._wv_lv_spangroup_t.safe_field("lines", 0))

    @property
    def indent(self):
        \"\"\"first line indent\"\"\"
        return int(self._wv_lv_spangroup_t.safe_field("indent", 0))

    @property
    def cache_w(self):
        \"\"\"the cache automatically calculates the width\"\"\"
        return int(self._wv_lv_spangroup_t.safe_field("cache_w", 0))

    @property
    def cache_h(self):
        \"\"\"similar cache_w\"\"\"
        return int(self._wv_lv_spangroup_t.safe_field("cache_h", 0))

    @property
    def child_ll(self):
        return safe_wrapper(self._wv_lv_spangroup_t, "child_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def overflow(self):
        \"\"\"details see lv_span_overflow_t\"\"\"
        return int(self._wv_lv_spangroup_t.safe_field("overflow", 0))

    @property
    def refresh(self):
        \"\"\"the spangroup need refresh cache_w and cache_h\"\"\"
        return int(self._wv_lv_spangroup_t.safe_field("refresh", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["lines"] = self.lines
        d["indent"] = self.indent
        d["cache_w"] = self.cache_w
        d["cache_h"] = self.cache_h
        d["child_ll"] = self.child_ll
        d["overflow"] = self.overflow
        d["refresh"] = self.refresh
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_spinbox.py'] = """\"\"\"
Auto-generated wrapper for lv_spinbox_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_textarea import LVTextarea


class LVSpinbox(LVTextarea):
    \"\"\"LVGL spinbox widget (lv_spinbox_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_spinbox_t = self.cast("lv_spinbox_t", ptr=True) or self
        self._wv = self._wv_lv_spinbox_t

    @property
    def value(self):
        return int(self._wv_lv_spinbox_t.safe_field("value", 0))

    @property
    def range_max(self):
        return int(self._wv_lv_spinbox_t.safe_field("range_max", 0))

    @property
    def range_min(self):
        return int(self._wv_lv_spinbox_t.safe_field("range_min", 0))

    @property
    def step(self):
        return int(self._wv_lv_spinbox_t.safe_field("step", 0))

    @property
    def digit_count(self):
        return int(self._wv_lv_spinbox_t.safe_field("digit_count", 0))

    @property
    def dec_point_pos(self):
        \"\"\"if 0, there is no separator and the number is an integer\"\"\"
        return int(self._wv_lv_spinbox_t.safe_field("dec_point_pos", 0))

    @property
    def rollover(self):
        \"\"\"Set to true for rollover functionality\"\"\"
        return int(self._wv_lv_spinbox_t.safe_field("rollover", 0))

    @property
    def digit_step_dir(self):
        \"\"\"the direction the digit will step on encoder button press when editing\"\"\"
        return int(self._wv_lv_spinbox_t.safe_field("digit_step_dir", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["value"] = self.value
        d["range_max"] = self.range_max
        d["range_min"] = self.range_min
        d["step"] = self.step
        d["digit_count"] = self.digit_count
        d["dec_point_pos"] = self.dec_point_pos
        d["rollover"] = self.rollover
        d["digit_step_dir"] = self.digit_step_dir
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_spinner.py'] = """\"\"\"
Auto-generated wrapper for lv_spinner_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from .lv_arc import LVArc


class LVSpinner(LVArc):
    \"\"\"LVGL spinner widget (lv_spinner_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_spinner_t = self.cast("lv_spinner_t", ptr=True) or self
        self._wv = self._wv_lv_spinner_t

    @property
    def duration(self):
        \"\"\"Anim duration in ms\"\"\"
        return int(self._wv_lv_spinner_t.safe_field("duration", 0))

    @property
    def angle(self):
        \"\"\"Anim angle in degrees\"\"\"
        return int(self._wv_lv_spinner_t.safe_field("angle", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["duration"] = self.duration
        d["angle"] = self.angle
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_switch.py'] = """\"\"\"
Auto-generated wrapper for lv_switch_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVSwitch(LVObject):
    \"\"\"LVGL switch widget (lv_switch_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_switch_t = self.cast("lv_switch_t", ptr=True) or self
        self._wv = self._wv_lv_switch_t

    @property
    def anim_state(self):
        return int(self._wv_lv_switch_t.safe_field("anim_state", 0))

    @property
    def orientation(self):
        \"\"\"Orientation of switch\"\"\"
        return int(self._wv_lv_switch_t.safe_field("orientation", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["anim_state"] = self.anim_state
        d["orientation"] = self.orientation
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_table.py'] = """\"\"\"
Auto-generated wrapper for lv_table_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVTable(LVObject):
    \"\"\"LVGL table widget (lv_table_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_table_t = self.cast("lv_table_t", ptr=True) or self
        self._wv = self._wv_lv_table_t

    @property
    def col_cnt(self):
        return int(self._wv_lv_table_t.safe_field("col_cnt", 0))

    @property
    def row_cnt(self):
        return int(self._wv_lv_table_t.safe_field("row_cnt", 0))

    @property
    def cell_data(self):
        return ptr_or_none(self._wv_lv_table_t.safe_field("cell_data"))

    @property
    def row_h(self):
        return ptr_or_none(self._wv_lv_table_t.safe_field("row_h"))

    @property
    def col_w(self):
        return ptr_or_none(self._wv_lv_table_t.safe_field("col_w"))

    @property
    def col_act(self):
        return int(self._wv_lv_table_t.safe_field("col_act", 0))

    @property
    def row_act(self):
        return int(self._wv_lv_table_t.safe_field("row_act", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["col_cnt"] = self.col_cnt
        d["row_cnt"] = self.row_cnt
        d["cell_data"] = self.cell_data
        d["row_h"] = self.row_h
        d["col_w"] = self.col_w
        d["col_act"] = self.col_act
        d["row_act"] = self.row_act
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_tabview.py'] = """\"\"\"
Auto-generated wrapper for lv_tabview_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVTabview(LVObject):
    \"\"\"LVGL tabview widget (lv_tabview_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_tabview_t = self.cast("lv_tabview_t", ptr=True) or self
        self._wv = self._wv_lv_tabview_t

    @property
    def tab_cur(self):
        return int(self._wv_lv_tabview_t.safe_field("tab_cur", 0))

    @property
    def tab_pos(self):
        return int(self._wv_lv_tabview_t.safe_field("tab_pos", 0))

    @property
    def tab_bar_size(self):
        return int(self._wv_lv_tabview_t.safe_field("tab_bar_size", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["tab_cur"] = self.tab_cur
        d["tab_pos"] = self.tab_pos
        d["tab_bar_size"] = self.tab_bar_size
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_textarea.py'] = """\"\"\"
Auto-generated wrapper for lv_textarea_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_point, safe_string


class LVTextarea(LVObject):
    \"\"\"LVGL textarea widget (lv_textarea_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_textarea_t = self.cast("lv_textarea_t", ptr=True) or self
        self._wv = self._wv_lv_textarea_t

    @property
    def label(self):
        \"\"\"Label of the text area\"\"\"
        return ptr_or_none(self._wv_lv_textarea_t.safe_field("label"))

    @property
    def placeholder_txt(self):
        \"\"\"Place holder label. only visible if text is an empty string\"\"\"
        return safe_string(self._wv_lv_textarea_t, "placeholder_txt")

    @property
    def placeholder_txt_size(self):
        \"\"\"Size of the placeholder text\"\"\"
        return safe_point(self._wv_lv_textarea_t, "placeholder_txt_size")

    @property
    def pwd_tmp(self):
        \"\"\"Used to store the original text in password mode\"\"\"
        return safe_string(self._wv_lv_textarea_t, "pwd_tmp")

    @property
    def pwd_bullet(self):
        \"\"\"Replacement characters displayed in password mode\"\"\"
        return safe_string(self._wv_lv_textarea_t, "pwd_bullet")

    @property
    def accepted_chars(self):
        \"\"\"Only these characters will be accepted. NULL: accept all\"\"\"
        return safe_string(self._wv_lv_textarea_t, "accepted_chars")

    @property
    def max_length(self):
        \"\"\"The max. number of characters. 0: no limit\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("max_length", 0))

    @property
    def pwd_show_time(self):
        \"\"\"Time to show characters in password mode before change them to '*'\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("pwd_show_time", 0))

    @property
    def sel_start(self):
        \"\"\"Temporary values for text selection\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("sel_start", 0))

    @property
    def sel_end(self):
        return int(self._wv_lv_textarea_t.safe_field("sel_end", 0))

    @property
    def text_sel_in_prog(self):
        \"\"\"User is in process of selecting\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("text_sel_in_prog", 0))

    @property
    def text_sel_en(self):
        \"\"\"Text can be selected on this text area\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("text_sel_en", 0))

    @property
    def pwd_mode(self):
        \"\"\"Replace characters with '*'\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("pwd_mode", 0))

    @property
    def one_line(self):
        \"\"\"One line mode (ignore line breaks)\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("one_line", 0))

    @property
    def static_accepted_chars(self):
        \"\"\"1: Only a pointer is saved in `accepted_chars`\"\"\"
        return int(self._wv_lv_textarea_t.safe_field("static_accepted_chars", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["label"] = self.label
        d["placeholder_txt"] = self.placeholder_txt
        d["placeholder_txt_size"] = self.placeholder_txt_size
        d["pwd_tmp"] = self.pwd_tmp
        d["pwd_bullet"] = self.pwd_bullet
        d["accepted_chars"] = self.accepted_chars
        d["max_length"] = self.max_length
        d["pwd_show_time"] = self.pwd_show_time
        d["sel_start"] = self.sel_start
        d["sel_end"] = self.sel_end
        d["text_sel_in_prog"] = self.text_sel_in_prog
        d["text_sel_en"] = self.text_sel_en
        d["pwd_mode"] = self.pwd_mode
        d["one_line"] = self.one_line
        d["static_accepted_chars"] = self.static_accepted_chars
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_tileview.py'] = """\"\"\"
Auto-generated wrapper for lv_tileview_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVTileview(LVObject):
    \"\"\"LVGL tileview widget (lv_tileview_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_tileview_t = self.cast("lv_tileview_t", ptr=True) or self
        self._wv = self._wv_lv_tileview_t

    @property
    def tile_act(self):
        return ptr_or_none(self._wv_lv_tileview_t.safe_field("tile_act"))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["tile_act"] = self.tile_act
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_tileview_tile.py'] = """\"\"\"
Auto-generated wrapper for lv_tileview_tile_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVTileviewTile(LVObject):
    \"\"\"LVGL tileview widget (lv_tileview_tile_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_tileview_tile_t = self.cast("lv_tileview_tile_t", ptr=True) or self
        self._wv = self._wv_lv_tileview_tile_t

    @property
    def dir(self):
        return int(self._wv_lv_tileview_tile_t.safe_field("dir", 0))

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dir"] = self.dir
        s['widget_data'] = d
        return s
"""

FILES['lvglgdb/lvgl/widgets/lv_win.py'] = """\"\"\"
Auto-generated wrapper for lv_win_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
\"\"\"

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVWin(LVObject):
    \"\"\"LVGL win widget (lv_win_t).\"\"\"

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_win_t = self.cast("lv_win_t", ptr=True) or self
        self._wv = self._wv_lv_win_t

    def snapshot(self, include_children=False, include_styles=False):
        \"\"\"Snapshot with widget-specific fields in widget_data.\"\"\"
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        return s
"""

FILES['lvglgdb/lvgl/widgets/specs.py'] = """\"\"\"Runtime access to the field metadata behind the generated wrappers.

The wrappers carry the values; what the values *mean* - which fields matter
first, what an enum's numbers are called, how a widget summarises itself - lives
in the generator's widget_specs.json. The dashboard already ships that file to
the browser, so reading the same file here keeps the terminal output and the
HTML inspector telling one story instead of two that drift apart.

Everything degrades to the raw value when the file is missing, which is the case
when the plugin is vendored without its scripts/ directory.
\"\"\"

import json
from pathlib import Path

_SPECS_PATH = (
    Path(__file__).resolve().parents[3] / "scripts" / "generators" / "widget_specs.json"
)

_specs = None


def _load() -> dict:
    global _specs
    if _specs is None:
        try:
            _specs = json.loads(_SPECS_PATH.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            _specs = {}
    return _specs


def widget_spec(class_name: str) -> dict:
    \"\"\"The spec for a class name, with or without the 'lv_' prefix.\"\"\"
    specs = _load()
    return specs.get(class_name) or specs.get("lv_" + class_name) or {}


def field_types(class_name: str) -> dict:
    \"\"\"Map of field name -> generator field type ('int', 'bool', 'enum:4', ...).\"\"\"
    return widget_spec(class_name).get("_auto", {}).get("fields", {})


def order_fields(class_name: str, keys) -> list:
    \"\"\"Order widget_data keys: the spec's primary fields first, rest after.\"\"\"
    primary = widget_spec(class_name).get("primary", [])
    keys = list(keys)
    head = [k for k in primary if k in keys]
    return head + [k for k in keys if k not in head]


def format_value(class_name: str, field: str, value) -> str:
    \"\"\"Render one widget_data value: enums by name, points and areas inline.\"\"\"
    if value is None:
        return "-"

    names = widget_spec(class_name).get("enums", {}).get(field)
    if names is not None and isinstance(value, int) and not isinstance(value, bool):
        if 0 <= value < len(names):
            return f"{names[value]} ({value})"
        return str(value)

    if isinstance(value, dict):
        if {"x1", "y1", "x2", "y2"} <= value.keys():
            return (
                f"({value['x1']},{value['y1']})-({value['x2']},{value['y2']})"
            )
        if {"x", "y"} <= value.keys():
            return f"({value['x']},{value['y']})"

    if field_types(class_name).get(field) == "bool":
        return str(bool(value))

    if isinstance(value, str):
        return f'"{escape(value)}"'

    return str(value)


def escape(text: str) -> str:
    \"\"\"Keep a value on one line: a label's text may well be several.\"\"\"
    return text.replace("\\\\", "\\\\\\\\").replace("\\n", "\\\\n").replace("\\r", "\\\\r").replace("\\t", "\\\\t")


def summary(class_name: str, widget_data: dict, limit: int = 32) -> str:
    \"\"\"One-line gist of a widget, from the spec's summary template.\"\"\"
    tpl = widget_spec(class_name).get("summary_tpl")
    if not tpl or not widget_data:
        return ""

    import re

    def _sub(m):
        value = widget_data.get(m.group(1))
        if value is None:
            return ""
        text = escape(str(value))
        return text[:limit] + "…" if len(text) > limit else text

    return re.sub(r"\\{(\\w+)\\}", _sub, tpl).strip()
"""

FILES['lvglgdb/value.py'] = """import gdb

from typing import Optional, Union


class Value(gdb.Value):
    def __init__(self, value: Union[gdb.Value, "Value"]):
        super().__init__(value)

    @property
    def is_ok(self) -> bool:
        \"\"\"True for valid Value, False for CorruptedValue.\"\"\"
        return True

    @staticmethod
    def normalize(val: "ValueInput", target_type: Optional[str] = None) -> "Value":
        \"\"\"Normalize input to a typed Value pointer.

        Args:
            val: Input value - str (GDB expression), int (address),
                 gdb.Value, or Value instance
            target_type: C type name (e.g. "lv_obj_t"). If provided,
                         result is cast to target_type*

        Returns:
            Value instance, optionally cast to target_type*

        Raises:
            ValueError: If target_type lookup fails or cast fails
            CorruptedError: If val is a CorruptedValue sentinel
        \"\"\"
        if isinstance(val, CorruptedValue):
            raise CorruptedError(val._addr, val._error)

        if isinstance(val, str):
            val = gdb.parse_and_eval(val)

        if isinstance(val, int):
            val = gdb.Value(val)

        if not isinstance(val, Value):
            val = Value(val)

        # Auto-infer target_type from pointer type when not specified
        if target_type is None:
            typ = val.type.strip_typedefs()
            if typ.code == gdb.TYPE_CODE_PTR:
                target = typ.target().strip_typedefs()
                if target.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
                    # Prefer .name or .tag over str() to avoid polluted
                    # anonymous struct expansions like "const struct {...}".
                    target_type = target.name or target.tag

        if target_type is not None:
            # If val is already a pointer whose target matches target_type,
            # keep the original type to avoid gdb.lookup_type() which may
            # resolve to a different compilation unit's definition when
            # macro-controlled fields cause multiple struct definitions.
            typ = val.type.strip_typedefs()
            if typ.code == gdb.TYPE_CODE_PTR:
                target = typ.target()
                tname = target.name or target.tag or ""
                if tname == target_type:
                    return val

            try:
                gdb.lookup_type(target_type)
            except gdb.error:
                raise ValueError(f"Type not found: {target_type}")

            if typ.code != gdb.TYPE_CODE_PTR:
                if typ.code in (gdb.TYPE_CODE_INT, gdb.TYPE_CODE_ENUM):
                    val = Value(val.cast(gdb.lookup_type(target_type).pointer()))
                else:
                    val = Value(val.address)

            result = val.cast(target_type, ptr=True)
            if result is None:
                raise ValueError(f"Failed to cast to {target_type}*")
            val = result

        return val

    # -- Memory error helpers --

    def _safe_addr(self) -> int:
        \"\"\"Extract address for diagnostic purposes, returning 0 on failure.\"\"\"
        try:
            return int(self)
        except (gdb.error, OverflowError):
            return 0

    def _is_memory_error(self, e: gdb.error) -> bool:
        \"\"\"Check if a gdb.error is actually a memory access failure.\"\"\"
        msg = str(e).lower()
        return any(k in msg for k in ("memory", "access", "cannot read"))

    def _wrap_memory_error(self, fn) -> Union["Value", "CorruptedValue"]:
        \"\"\"Call fn, converting memory errors to CorruptedValue.\"\"\"
        try:
            return fn()
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise

    def read_value(self) -> Union["Value", "CorruptedValue"]:
        \"\"\"Force a memory read probe. Returns CorruptedValue if unreadable.\"\"\"
        try:
            addr = int(gdb.Value.__int__(self))
        except (gdb.MemoryError, gdb.error):
            return CorruptedValue(self._safe_addr(), gdb.MemoryError(
                f"Cannot access memory at address {hex(self._safe_addr())}"))
        if addr:
            try:
                gdb.selected_inferior().read_memory(addr, 1)
            except gdb.MemoryError as e:
                return CorruptedValue(addr, e)
            except gdb.error as e:
                if self._is_memory_error(e):
                    return CorruptedValue(addr, e)
                raise
        return self

    # -- Hooked GDB access methods --

    def __bool__(self) -> bool:
        \"\"\"Wrap gdb.Value truthiness check to handle memory errors.\"\"\"
        try:
            return bool(gdb.Value.__bool__(self))
        except gdb.MemoryError:
            return False
        except gdb.error as e:
            if self._is_memory_error(e):
                return False
            msg = str(e).lower()
            if "cannot convert" in msg:
                return False
            raise

    def __int__(self) -> int:
        \"\"\"Wrap gdb.Value int conversion to handle memory errors.\"\"\"
        try:
            return int(gdb.Value.__int__(self))
        except gdb.MemoryError:
            return 0
        except gdb.error as e:
            if self._is_memory_error(e):
                return 0
            # "Cannot convert value to long" happens when a corrupted
            # pointer dereferences into a struct that GDB cannot coerce
            # to an integer.  Treat as zero rather than propagating.
            msg = str(e).lower()
            if "cannot convert" in msg:
                return 0
            raise

    def __getitem__(self, key):
        try:
            raw = gdb.Value.__getitem__(self, key)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def __getattr__(self, key):
        try:
            if hasattr(gdb.Value, key):
                raw = gdb.Value.__getattribute__(self, key)
            else:
                raw = gdb.Value.__getitem__(self, key)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def dereference(self) -> Union["Value", "CorruptedValue"]:
        try:
            raw = gdb.Value.dereference(self)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def cast(
        self, type_name: Union[str, gdb.Type], ptr: bool = False
    ) -> Optional[Union["Value", "CorruptedValue"]]:
        try:
            gdb_type = (
                gdb.lookup_type(type_name) if isinstance(type_name, str) else type_name
            )
            if ptr:
                gdb_type = gdb_type.pointer()
            return Value(gdb.Value.cast(self, gdb_type))
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            return None  # Non-memory error preserves original behavior

    def super_value(self, attr: str) -> "Value":
        return self[attr]

    def safe_field(self, attr: str, default=None, cast=None):
        \"\"\"Access a struct field that may not exist in older LVGL versions.

        Returns the field value (optionally converted by *cast*), or
        *default* when the field is missing.
        \"\"\"
        try:
            v = Value(gdb.Value.__getitem__(self, attr))
            return cast(int(v)) if cast is not None else v
        except (gdb.error, gdb.MemoryError):
            return default

    def string(self, *args, fallback=None, **kwargs) -> str:
        \"\"\"Read a C string, returning fallback on memory error.\"\"\"
        try:
            return gdb.Value.string(self, *args, **kwargs)
        except gdb.MemoryError:
            return fallback if fallback is not None else f"(corrupted@{hex(self._safe_addr())})"
        except gdb.error as e:
            if self._is_memory_error(e):
                return fallback if fallback is not None else f"(corrupted@{hex(self._safe_addr())})"
            raise

    def __str__(self):
        \"\"\"Provide better string representation for debugging\"\"\"
        try:
            ptr_val = int(self)
            return f"({self.type})0x{ptr_val:x}"
        except gdb.error:
            pass

        return super().__str__()

    def __repr__(self):
        \"\"\"Provide detailed representation\"\"\"
        try:
            content = self.dereference()
            if isinstance(content, CorruptedValue):
                return f"Value({self.__str__()}: {content!r})"
            return f"Value({self.__str__()}: {content})"
        except gdb.error:
            pass

        return f"Value({self.__str__()})"


class CorruptedError(Exception):
    \"\"\"Raised when a CorruptedValue field is accessed (terminal operation).\"\"\"

    def __init__(self, addr: int, original_error: Exception):
        self.addr = addr
        self.original_error = original_error
        super().__init__(
            f"Corrupted value @{hex(addr)}: {original_error}"
        )


class CorruptedValue:
    \"\"\"Sentinel for inaccessible memory. Infectious through chained ops.

    Not a subclass of Value or gdb.Value. Propagates through dereference()
    and cast(), terminates with CorruptedError on field access.
    bool(CorruptedValue) == False enables natural loop termination.
    \"\"\"

    def __init__(self, addr: int, error: Exception):
        object.__setattr__(self, "_addr", addr)
        object.__setattr__(self, "_error", error)

    # -- Safe exits: never raise --

    @property
    def is_ok(self) -> bool:
        \"\"\"Always False for CorruptedValue.\"\"\"
        return False

    def __int__(self) -> int:
        return self._addr

    def __bool__(self) -> bool:
        return False

    def snapshot(self):
        \"\"\"Return a corrupted Snapshot with diagnostic info.\"\"\"
        from .lvgl.snapshot import Snapshot
        d = {
            "addr": hex(self._addr),
            "class_name": "(corrupted)",
            "error": str(self._error),
        }
        return Snapshot(d, source=self)

    def __repr__(self) -> str:
        return f"CorruptedValue(@{hex(self._addr)}: {self._error})"

    def __str__(self) -> str:
        return f"(corrupted@{hex(self._addr)})"

    # -- Propagating ops: return self, never raise --

    def read_value(self):
        return self

    def dereference(self):
        return self

    def cast(self, *args, **kwargs):
        return self

    def string(self, *args, fallback=None, **kwargs) -> str:
        \"\"\"Return fallback string for corrupted value.\"\"\"
        return fallback if fallback is not None else f"(corrupted@{hex(self._addr)})"

    # -- Terminal ops: raise CorruptedError / AttributeError --

    def __getattr__(self, key):
        if key.startswith("__") and key.endswith("__"):
            raise AttributeError(key)
        raise CorruptedError(self._addr, self._error)

    def __getitem__(self, key):
        raise CorruptedError(self._addr, self._error)


# Type alias for all wrapper class __init__ parameters
ValueInput = Union[str, int, gdb.Value, Value]
"""

FILES['scripts/generators/widget_specs.json'] = """{
  "_comment": "Widget spec overrides: summary templates, primary fields, enum names. Generator merges with auto-detected field types.",
  "lv_3dtexture": {
    "_auto": {
      "fields": {
        "id": "int",
        "h_flip": "bool",
        "v_flip": "bool"
      }
    }
  },
  "lv_animimg": {
    "_auto": {
      "fields": {
        "dsc": "pointer",
        "pic_count": "int"
      }
    },
    "summary_tpl": "",
    "primary": [
      "pic_count"
    ]
  },
  "lv_arc": {
    "_auto": {
      "fields": {
        "rotation": "int",
        "indic_angle_start": "int",
        "indic_angle_end": "int",
        "bg_angle_start": "int",
        "bg_angle_end": "int",
        "value": "int",
        "min_value": "int",
        "max_value": "int",
        "dragging": "bool",
        "type": "enum:2",
        "min_close": "bool",
        "in_out": "bool",
        "chg_rate": "int",
        "last_tick": "int",
        "last_angle": "int",
        "knob_offset": "int"
      }
    },
    "summary_tpl": "{value} [{min_value}..{max_value}]",
    "primary": [
      "value",
      "min_value",
      "max_value",
      "rotation",
      "type"
    ],
    "enums": {
      "type": [
        "NORMAL",
        "SYMMETRIC",
        "REVERSE"
      ]
    }
  },
  "lv_arclabel": {
    "_auto": {
      "fields": {
        "text": "string",
        "dot_begin": "int",
        "angle_start": "int",
        "angle_size": "int",
        "offset": "int",
        "radius": "int",
        "center_offset": "point",
        "dir": "int",
        "text_align_v": "int",
        "text_align_h": "int",
        "static_txt": "bool",
        "recolor": "bool",
        "overflow": "enum:2",
        "end_overlap": "bool"
      }
    },
    "summary_tpl": "{text}",
    "primary": [
      "text",
      "angle_start",
      "angle_size",
      "radius",
      "dir"
    ],
    "enums": {
      "dir": [
        "CW",
        "CCW"
      ],
      "overflow": [
        "VISIBLE",
        "ELLIPSIS",
        "CLIP"
      ]
    }
  },
  "lv_bar": {
    "_auto": {
      "fields": {
        "cur_value": "int",
        "min_value": "int",
        "max_value": "int",
        "start_value": "int",
        "indic_area": "area",
        "val_reversed": "bool",
        "mode": "enum:3",
        "orientation": "enum:3"
      }
    },
    "summary_tpl": "{cur_value}/{max_value}",
    "primary": [
      "cur_value",
      "min_value",
      "max_value",
      "start_value",
      "mode"
    ],
    "enums": {
      "mode": [
        "NORMAL",
        "SYMMETRICAL",
        "RANGE"
      ],
      "orientation": [
        "AUTO",
        "HORIZONTAL",
        "VERTICAL"
      ]
    }
  },
  "lv_barcode": {
    "_auto": {
      "fields": {
        "dark_color": "color",
        "light_color": "color",
        "data": "string",
        "pattern": "pointer",
        "bar_count": "int",
        "scale": "int",
        "direction": "int",
        "encoding": "int",
        "tiled": "bool",
        "update_mode": "bool",
        "needs_update": "bool",
        "render_valid": "bool",
        "fitting": "bool"
      }
    }
  },
  "lv_button": {
    "_auto": {
      "fields": {}
    }
  },
  "lv_buttonmatrix": {
    "_auto": {
      "fields": {
        "map_p": "pointer",
        "button_areas": "pointer",
        "ctrl_bits": "pointer",
        "btn_cnt": "int",
        "row_cnt": "int",
        "btn_id_sel": "int",
        "one_check": "bool",
        "auto_free_map": "bool"
      }
    },
    "summary_tpl": "{btn_cnt} btns",
    "primary": [
      "btn_cnt",
      "row_cnt",
      "btn_id_sel",
      "one_check"
    ]
  },
  "lv_calendar": {
    "_auto": {
      "fields": {
        "btnm": "pointer",
        "highlighted_dates": "pointer",
        "highlighted_dates_num": "int",
        "use_chinese_calendar": "bool"
      }
    },
    "summary_tpl": "",
    "primary": [
      "today",
      "showed_date",
      "use_chinese_calendar"
    ]
  },
  "lv_canvas": {
    "_auto": {
      "fields": {
        "draw_buf": "pointer"
      }
    },
    "summary_tpl": "",
    "primary": [
      "draw_buf"
    ]
  },
  "lv_chart": {
    "_auto": {
      "fields": {
        "pressed_point_id": "int",
        "hdiv_cnt": "int",
        "vdiv_cnt": "int",
        "point_cnt": "int",
        "type": "enum:4",
        "update_mode": "enum:2"
      }
    },
    "summary_tpl": "{point_cnt}pts",
    "primary": [
      "point_cnt",
      "hdiv_cnt",
      "vdiv_cnt",
      "type"
    ],
    "enums": {
      "type": [
        "NONE",
        "LINE",
        "BAR",
        "SCATTER"
      ],
      "update_mode": [
        "SHIFT",
        "CIRCULAR"
      ]
    }
  },
  "lv_checkbox": {
    "_auto": {
      "fields": {
        "txt": "string",
        "static_txt": "bool"
      }
    },
    "summary_tpl": "{txt}",
    "primary": [
      "txt",
      "static_txt"
    ]
  },
  "lv_dropdown": {
    "_auto": {
      "fields": {
        "list": "pointer",
        "text": "string",
        "symbol": "pointer",
        "options": "string",
        "text_translation_tag": "string",
        "options_translation_tag": "string",
        "option_cnt": "int",
        "sel_opt_id": "int",
        "sel_opt_id_orig": "int",
        "pr_opt_id": "int",
        "dir": "enum:4",
        "static_options": "bool",
        "selected_highlight": "bool",
        "static_text": "bool"
      }
    },
    "summary_tpl": "sel={sel_opt_id}/{option_cnt}",
    "primary": [
      "options",
      "option_cnt",
      "sel_opt_id",
      "dir"
    ],
    "enums": {
      "dir": [
        "TOP",
        "BOTTOM",
        "LEFT",
        "RIGHT"
      ]
    }
  },
  "lv_dropdown_list": {
    "_auto": {
      "fields": {
        "dropdown": "pointer"
      }
    }
  },
  "lv_ffmpeg_player": {
    "_auto": {
      "fields": {
        "timer": "pointer",
        "auto_restart": "bool",
        "decoder_name": "string"
      }
    }
  },
  "lv_gstreamer": {
    "_auto": {
      "fields": {
        "last_buffer": "pointer",
        "last_sample": "pointer",
        "pipeline": "pointer",
        "audio_convert": "pointer",
        "video_convert": "pointer",
        "audio_volume": "pointer",
        "aligned_frame": "pointer",
        "color_format": "int",
        "gstreamer_timer": "pointer",
        "frame_queue": "pointer",
        "is_video_info_valid": "bool"
      }
    }
  },
  "lv_image": {
    "_auto": {
      "fields": {
        "src": "pointer",
        "bitmap_mask_src": "pointer",
        "offset": "point",
        "w": "int",
        "h": "int",
        "rotation": "int",
        "scale_x": "int",
        "scale_y": "int",
        "pivot": "point",
        "src_type": "enum:2",
        "cf": "enum:5",
        "antialias": "bool",
        "align": "enum:4",
        "blend_mode": "enum:4"
      }
    },
    "summary_tpl": "{w}×{h}",
    "primary": [
      "src",
      "w",
      "h",
      "rotation",
      "scale_x",
      "scale_y",
      "align"
    ],
    "enums": {
      "src_type": [
        "VARIABLE",
        "FILE",
        "SYMBOL"
      ],
      "align": [
        "DEFAULT",
        "TOP_LEFT",
        "TOP_MID",
        "TOP_RIGHT",
        "BOTTOM_LEFT",
        "BOTTOM_MID",
        "BOTTOM_RIGHT",
        "LEFT_MID",
        "RIGHT_MID",
        "CENTER",
        "STRETCH",
        "TILE"
      ],
      "blend_mode": [
        "NORMAL",
        "ADDITIVE",
        "SUBTRACTIVE",
        "MULTIPLY"
      ]
    }
  },
  "lv_imagebutton": {
    "_auto": {
      "fields": {}
    }
  },
  "lv_ime_pinyin": {
    "_auto": {
      "fields": {
        "kb": "pointer",
        "cand_panel": "pointer",
        "dict": "pointer",
        "cand_str": "string",
        "k9_py_ll_pos": "int",
        "k9_legal_py_count": "int",
        "k9_input_str_len": "int",
        "ta_count": "int",
        "cand_num": "int",
        "py_page": "int",
        "mode": "int"
      }
    }
  },
  "lv_keyboard": {
    "_auto": {
      "fields": {
        "ta": "pointer",
        "mode": "int",
        "popovers": "bool"
      }
    },
    "summary_tpl": "mode={mode}",
    "primary": [
      "ta",
      "mode",
      "popovers"
    ],
    "enums": {
      "mode": [
        "TEXT_LOWER",
        "TEXT_UPPER",
        "SPECIAL",
        "NUMBER",
        "USER_1",
        "USER_2",
        "USER_3",
        "USER_4"
      ]
    }
  },
  "lv_label": {
    "_auto": {
      "fields": {
        "text": "string",
        "translation_tag": "string",
        "dot_begin": "int",
        "max_lines": "int",
        "sel_start": "int",
        "sel_end": "int",
        "size_cache": "point",
        "offset": "point",
        "long_mode": "enum:4",
        "static_txt": "bool",
        "recolor": "bool",
        "expand": "bool",
        "invalid_size_cache": "bool",
        "need_refr_text": "bool",
        "text_size": "point"
      }
    },
    "summary_tpl": "{text}",
    "primary": [
      "text",
      "long_mode",
      "recolor",
      "static_txt"
    ],
    "enums": {
      "long_mode": [
        "WRAP",
        "DOT",
        "SCROLL",
        "CLIP",
        "SCROLL_CIRCULAR"
      ]
    }
  },
  "lv_led": {
    "_auto": {
      "fields": {
        "color": "color",
        "bright": "int"
      }
    },
    "summary_tpl": "bright={bright}",
    "primary": [
      "color",
      "bright"
    ]
  },
  "lv_line": {
    "_auto": {
      "fields": {
        "point_num": "int",
        "y_inv": "bool",
        "point_array_is_mutable": "bool"
      }
    }
  },
  "lv_menu": {
    "_auto": {
      "fields": {
        "storage": "pointer",
        "main": "pointer",
        "main_page": "pointer",
        "main_header": "pointer",
        "main_header_title": "pointer",
        "sidebar": "pointer",
        "sidebar_page": "pointer",
        "sidebar_header": "pointer",
        "sidebar_header_title": "pointer",
        "selected_tab": "pointer",
        "cur_depth": "int",
        "prev_depth": "int",
        "sidebar_generated": "bool",
        "mode_header": "enum:3",
        "mode_root_back_btn": "enum:2"
      }
    },
    "summary_tpl": "",
    "primary": [
      "cur_depth",
      "sidebar_generated"
    ]
  },
  "lv_menu_page": {
    "_auto": {
      "fields": {
        "title": "string",
        "static_title": "bool"
      }
    },
    "summary_tpl": "{title}",
    "primary": [
      "title",
      "static_title"
    ]
  },
  "lv_msgbox": {
    "_auto": {
      "fields": {
        "header": "pointer",
        "content": "pointer",
        "footer": "pointer",
        "title": "pointer",
        "auto_parent": "bool"
      }
    },
    "summary_tpl": "",
    "primary": [
      "header",
      "content",
      "footer",
      "title"
    ]
  },
  "lv_qrcode": {
    "_auto": {
      "fields": {
        "dark_color": "color",
        "light_color": "color",
        "data": "pointer",
        "data_len": "enum:12",
        "quiet_zone": "bool",
        "update_mode": "bool",
        "needs_update": "bool",
        "render_valid": "bool"
      }
    }
  },
  "lv_roller": {
    "_auto": {
      "fields": {
        "options_translation_tag": "string",
        "option_cnt": "int",
        "sel_opt_id": "int",
        "sel_opt_id_ori": "int",
        "inf_page_cnt": "int",
        "mode": "enum:2",
        "moved": "bool"
      }
    },
    "summary_tpl": "sel={sel_opt_id}/{option_cnt}",
    "primary": [
      "option_cnt",
      "sel_opt_id",
      "mode"
    ],
    "enums": {
      "mode": [
        "NORMAL",
        "INFINITE"
      ]
    }
  },
  "lv_scale": {
    "_auto": {
      "fields": {
        "txt_src": "pointer",
        "mode": "int",
        "range_min": "int",
        "range_max": "int",
        "total_tick_count": "enum:15",
        "major_tick_every": "enum:15",
        "label_enabled": "bool",
        "post_draw": "bool",
        "draw_ticks_on_top": "bool",
        "angle_range": "int",
        "rotation": "int",
        "custom_label_cnt": "int",
        "last_tick_width": "int",
        "first_tick_width": "int"
      }
    },
    "summary_tpl": "{range_min}..{range_max}",
    "primary": [
      "mode",
      "range_min",
      "range_max",
      "total_tick_count",
      "angle_range",
      "rotation"
    ],
    "enums": {
      "mode": [
        "HORIZONTAL_TOP",
        "HORIZONTAL_BOTTOM",
        "VERTICAL_LEFT",
        "VERTICAL_RIGHT",
        "ROUND_INNER",
        "ROUND_OUTER"
      ]
    }
  },
  "lv_slider": {
    "_auto": {
      "fields": {
        "left_knob_area": "area",
        "right_knob_area": "area",
        "pressed_point": "point",
        "value_to_set": "pointer",
        "dragging": "bool",
        "left_knob_focus": "bool"
      }
    },
    "summary_tpl": "{cur_value}/{max_value}",
    "primary": [
      "cur_value",
      "min_value",
      "max_value",
      "mode",
      "dragging"
    ]
  },
  "lv_spangroup": {
    "_auto": {
      "fields": {
        "lines": "int",
        "indent": "int",
        "cache_w": "int",
        "cache_h": "int",
        "overflow": "bool",
        "refresh": "bool"
      }
    },
    "summary_tpl": "",
    "primary": [
      "lines",
      "indent"
    ]
  },
  "lv_spinbox": {
    "_auto": {
      "fields": {
        "value": "int",
        "range_max": "int",
        "range_min": "int",
        "step": "int",
        "digit_count": "enum:4",
        "dec_point_pos": "enum:4",
        "rollover": "bool",
        "digit_step_dir": "enum:2"
      }
    },
    "summary_tpl": "{value}",
    "primary": [
      "value",
      "range_min",
      "range_max",
      "step",
      "digit_count",
      "dec_point_pos"
    ]
  },
  "lv_spinner": {
    "_auto": {
      "fields": {
        "duration": "int",
        "angle": "int"
      }
    },
    "summary_tpl": "{duration}ms",
    "primary": [
      "duration",
      "angle"
    ]
  },
  "lv_switch": {
    "_auto": {
      "fields": {
        "anim_state": "int",
        "orientation": "enum:3"
      }
    },
    "summary_tpl": "",
    "primary": [
      "anim_state",
      "orientation"
    ],
    "enums": {
      "orientation": [
        "AUTO",
        "HORIZONTAL",
        "VERTICAL"
      ]
    }
  },
  "lv_table": {
    "_auto": {
      "fields": {
        "col_cnt": "int",
        "row_cnt": "int",
        "cell_data": "pointer",
        "row_h": "pointer",
        "col_w": "pointer",
        "col_act": "int",
        "row_act": "int"
      }
    },
    "summary_tpl": "{col_cnt}×{row_cnt}",
    "primary": [
      "col_cnt",
      "row_cnt",
      "col_act",
      "row_act"
    ]
  },
  "lv_tabview": {
    "_auto": {
      "fields": {
        "tab_cur": "int",
        "tab_pos": "int",
        "tab_bar_size": "int"
      }
    },
    "summary_tpl": "tab={tab_cur}",
    "primary": [
      "tab_cur",
      "tab_pos",
      "tab_bar_size"
    ]
  },
  "lv_textarea": {
    "_auto": {
      "fields": {
        "label": "pointer",
        "placeholder_txt": "string",
        "placeholder_txt_size": "point",
        "pwd_tmp": "string",
        "pwd_bullet": "string",
        "accepted_chars": "string",
        "max_length": "int",
        "pwd_show_time": "int",
        "sel_start": "int",
        "sel_end": "int",
        "text_sel_in_prog": "bool",
        "text_sel_en": "bool",
        "pwd_mode": "bool",
        "one_line": "bool",
        "static_accepted_chars": "bool"
      }
    },
    "summary_tpl": "{placeholder_txt}",
    "primary": [
      "placeholder_txt",
      "max_length",
      "pwd_show_time",
      "accepted_chars"
    ]
  },
  "lv_tileview": {
    "_auto": {
      "fields": {
        "tile_act": "pointer"
      }
    },
    "summary_tpl": "",
    "primary": [
      "tile_act"
    ]
  },
  "lv_tileview_tile": {
    "_auto": {
      "fields": {
        "dir": "int"
      }
    },
    "summary_tpl": "",
    "primary": [
      "dir"
    ]
  },
  "lv_win": {
    "_auto": {
      "fields": {}
    }
  }
}
"""


def _load():
    import os
    import sys

    if sys.platform == "win32":
        base = os.environ.get("LOCALAPPDATA") or os.path.expanduser("~")
    else:
        base = os.environ.get("XDG_CACHE_HOME") or os.path.expanduser("~/.cache")
    # Per user, not the shared temp directory, so nobody else can plant code
    # there; and per content hash, so different versions never mix.
    root = os.path.join(base, "lvglgdb", f"{VERSION}-{DIGEST}")
    done = os.path.join(root, ".complete")

    if not os.path.exists(done):
        for rel, text in FILES.items():
            path = os.path.join(root, *rel.split("/"))
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8", newline="\n") as f:
                f.write(text)
        open(done, "w").close()

    # Sourcing the bundle again reloads the plugin, like gdbinit.py does.
    for name in [m for m in sys.modules if m == "lvglgdb" or m.startswith("lvglgdb.")]:
        del sys.modules[name]
    if root in sys.path:
        sys.path.remove(root)
    sys.path.insert(0, root)

    import lvglgdb  # noqa: F401


_load()
