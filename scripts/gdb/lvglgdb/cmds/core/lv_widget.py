"""`info widget`: everything known about a widget, in a readable list.

`dump obj` walks the tree but prints one line per widget, and `info obj_class`
answers about the class, not the instance in front of you. Between them the
per-widget fields the wrappers collect - a label's text, an arc's angles - were
only reachable through `dump dashboard`. This command prints the same data the
dashboard shows, for one widget or for every widget on every display - and,
given field names, only the fields asked for.

`dump widget props` answers the question that comes right after: which field
names does this widget have? It reads them from the generated wrapper, so a
class name is enough - no live object needed.
"""

import argparse

import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_obj import LVObject
from lvglgdb.lvgl.misc.lv_style import read_symbol
from lvglgdb.lvgl.misc.lv_utils import format_enum
from lvglgdb.lvgl.widgets import specs
from lvglgdb.value import CorruptedError

_LABEL_W = 14


def _snapshot(obj) -> dict:
    """Snapshot of a single widget, widget_data included, children excluded."""
    return LVObject._wrap_as_widget(obj).snapshot().as_dict()


def _field(label, value, indent=1):
    print(f"{'  ' * indent}{label:{_LABEL_W}s} = {value}")


def _addr_str(addr) -> str:
    """A pointer with its C symbol, when the address has one."""
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


def _headline(d) -> str:
    name = d.get("name")
    gist = specs.summary(d.get("class_name", ""), d.get("widget_data"))
    parts = [f"{d.get('class_name', '?')} @{d.get('addr', '?')}"]
    if name:
        parts.append(f"name={name}")
    if gist:
        parts.append(f'"{gist}"' if not gist.startswith('"') else gist)
    return "  ".join(parts)


def _compact(d) -> str:
    """One line for the tree listing."""
    line = _headline(d)
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
    """Event callbacks, with the symbol GDB resolves each one to."""
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
    """The widget's own struct fields, primary ones first."""
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
    """An object-level field by name, accepting a few friendly aliases."""
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
    """Render an object-level field the way the full listing renders it."""
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
    """Print only the named fields of one widget."""
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
    """Print everything known about one widget."""
    d = _snapshot(obj)

    print(_headline(d))
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
                print(f"    [{i}] {_compact(_snapshot(child))}")
            except CorruptedError as e:
                print(f"    [{i}] (corrupted: {e})")


def print_tree(obj, depth=0, limit=None):
    """Print one compact line per widget, indented by tree depth."""
    try:
        print("  " * depth + _compact(_snapshot(obj)))
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
    """List every widget of every display, screens labelled with their layer."""
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
    """show all details of a widget, or list every widget"""

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
            help="List the subtree of <expr> instead of its details.",
        )
        parser.add_argument(
            "-L",
            "--level",
            type=int,
            default=None,
            help="Limit the depth of a listing.",
        )
        parser.add_argument(
            "expr",
            type=str,
            nargs="?",
            default=None,
            help="A widget: a variable, a member chain, or its address.",
        )
        parser.add_argument(
            "fields",
            type=str,
            nargs="*",
            help="Field names to print instead of the whole widget.",
        )
        try:
            opts = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if opts.expr:
            try:
                obj = LVObject(gdb.parse_and_eval(opts.expr))
            except gdb.error as e:
                print(f"Error: {e}")
                return
            if opts.tree:
                print_tree(obj, limit=opts.level)
            elif opts.fields:
                print_widget_fields(obj, opts.fields)
            else:
                print_widget(obj, expr=opts.expr)
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
    """The generated wrapper for a class name, or None."""
    from lvglgdb.lvgl.widgets import WIDGET_REGISTRY

    return WIDGET_REGISTRY.get(class_name) or WIDGET_REGISTRY.get("lv_" + class_name)


def _class_name_of(cls):
    """'lv_bar' for the wrapper defined in widgets/lv_bar.py."""
    return cls.__module__.rsplit(".", 1)[-1]


def print_widget_props(class_name, live_fields=None):
    """List the field names a widget has, grouped by the struct they come from."""
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
    """Snapshot keys as the names to type: the aliases _obj_field() accepts.

    A snapshot carries both `flags` and `flags_list`; only one of them is worth
    listing as a field name, and it is the short one.
    """
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
    """widget related dumps"""

    def __init__(self):
        super(DumpWidget, self).__init__(
            "dump widget", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION, prefix=True
        )


class DumpWidgetProps(gdb.Command):
    """list the field names of a widget class or of a live widget"""

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

        try:
            obj = LVObject(gdb.parse_and_eval(opts.target))
            d = _snapshot(obj)
        except (gdb.error, CorruptedError):
            print(f"'{opts.target}' is neither a widget class nor a widget.")
            print_widget_classes()
            return
        live = _friendly_names(
            k for k in d if k not in ("widget_data", "children", "styles")
        )
        print_widget_props(d.get("class_name", ""), live_fields=live)
