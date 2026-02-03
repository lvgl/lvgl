from lvglgdb.value import Value


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
            child = self.spec_attr.children[i]
            yield LVObject(child)

    @property
    def styles(self):
        LV_STYLE_PROP_INV = 0
        LV_STYLE_PROP_ANY = 0xFF
        count = self.style_cnt
        if count == 0:
            return

        styles = self.super_value("styles")
        for i in range(count):
            style = styles[i].style
            prop_cnt = style.prop_cnt
            values_and_props = style.values_and_props.cast(
                "lv_style_const_prop_t", ptr=True
            )
            for j in range(prop_cnt):
                prop = values_and_props[j].prop
                if prop == LV_STYLE_PROP_INV or prop == LV_STYLE_PROP_ANY:
                    continue
                yield values_and_props[j]

    def get_child(self, index: int):
        return self.spec_attr.children[index] if self.spec_attr else None


def dump_obj_info(obj: LVObject):
    clzname = obj.class_name
    coords = f"{obj.x1},{obj.y1},{obj.x2},{obj.y2}"
    print(f"{clzname}@{hex(obj)} {coords}")
