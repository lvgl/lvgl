from lvglgdb.value import Value
from lvglgdb.lvgl.misc.lv_style import LVStyle, StyleEntry, decode_selector


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


class LVObject(Value):
    """LVGL object"""

    def __init__(self, obj: Value):
        super().__init__(obj.cast("lv_obj_t", ptr=True))

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


def dump_obj_info(obj: LVObject):
    clzname = obj.class_name
    coords = f"{obj.x1},{obj.y1},{obj.x2},{obj.y2}"
    print(f"{clzname}@{hex(obj)} {coords}")


def dump_obj_styles(obj: Value):
    """Print all styles of an object, reusing LVStyle.print_entries()."""
    lv_obj = LVObject(Value(obj))

    has_any = False
    for obj_style in lv_obj.obj_styles:
        has_any = True
        print(f"[{obj_style.index}] {obj_style.selector_str}  {obj_style.flags_str}")
        obj_style.style.print_entries()

    if not has_any:
        print("No styles applied.")
