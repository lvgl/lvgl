from lvglgdb.value import Value, ValueInput
from lvglgdb.lvgl.misc.lv_style import LVStyle, decode_selector


class ObjStyle:
    """A single style slot from obj->styles[], wrapping LVStyle."""

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
    """LVGL object"""

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
    def class_name(self):
        name = self.class_p.name
        if name:
            return name.string()
        return self.class_p.format_string(symbols=True, address=True, styling=True)

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
    def child_count(self):
        return self.spec_attr.child_cnt if self.spec_attr else 0

    @property
    def event_list(self):
        """Get event list from obj->spec_attr->event_list."""
        from ..misc.lv_event import LVEventList

        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        return LVEventList(spec.event_list)

    @property
    def children(self):
        if not self.spec_attr:
            return
        for i in range(self.child_count):
            yield LVObject(self.spec_attr.children[i])

    @property
    def obj_styles(self):
        """Yield ObjStyle for each entry in obj->styles[]."""
        count = int(self.style_cnt)
        if count == 0:
            return
        styles_arr = self.super_value("styles")
        for i in range(count):
            raw = styles_arr[i]
            flags = []
            if int(raw.is_local):
                flags.append("local")
            if int(raw.is_trans):
                flags.append("trans")
            if int(raw.is_theme):
                flags.append("theme")
            if int(raw.is_disabled):
                flags.append("disabled")
            yield ObjStyle(i, int(raw.selector), flags, LVStyle(raw.style))

    @property
    def styles(self):
        """Yield StyleEntry for all style properties across all slots."""
        for obj_style in self.obj_styles:
            yield from obj_style

    def get_child(self, index: int):
        return self.spec_attr.children[index] if self.spec_attr else None

    def snapshot(self, include_children=False, include_styles=False):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import ptr_or_none

        d = {
            "addr": hex(int(self)),
            "class_name": self.class_name,
            "coords": {
                "x1": self.x1,
                "y1": self.y1,
                "x2": self.x2,
                "y2": self.y2,
            },
            "child_count": int(self.child_count),
            "style_count": int(self.style_cnt),
            "parent_addr": ptr_or_none(self.super_value("parent")),
            "group_addr": self._get_group_addr(),
        }
        if include_children:
            d["children"] = [
                c.snapshot(include_children=True, include_styles=include_styles).as_dict()
                for c in self.children
            ]
        if include_styles:
            d["styles"] = [s.snapshot().as_dict() for s in self.obj_styles]
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    def _get_group_addr(self):
        """Get group address from spec_attr, or None."""
        spec = self.spec_attr
        if not spec or not int(spec):
            return None
        try:
            grp = spec.group
            addr = int(grp)
            return hex(addr) if addr else None
        except Exception:
            return None


def dump_obj_info(obj: LVObject):
    from lvglgdb.lvgl.formatter import print_info
    print_info(obj.snapshot())


def dump_obj_styles(obj: ValueInput):
    """Print all styles of an object."""
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
            value_str = f"{value_str} \033[48;2;{r};{g};{b}m  \033[0m"
        return [entry["prop_name"], value_str]

    for s in styles:
        print(f"[{s['index']}] {s['selector_str']}  {s['flags_str']}")
        print_table(
            s.get("properties", []), ["prop", "value"], _style_row,
            "Empty style.", align="l", numbered=False,
        )
