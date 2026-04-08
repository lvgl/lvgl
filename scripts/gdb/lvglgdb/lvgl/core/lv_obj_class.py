import gdb

from lvglgdb.value import Value, ValueInput
from ..misc.lv_utils import format_coord


class LVObjClass(Value):
    """LVGL object class wrapper"""

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
            ("theme_inh", "theme_inheritable"),
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
        """Collect all lv_obj_class_t globals from the symbol table."""
        import re

        # Search by symbol name suffix; filter by type to exclude non-class matches
        output = gdb.execute("info variables _class$", to_string=True)
        classes = []
        for line in output.splitlines():
            if "lv_obj_class_t" not in line:
                continue
            m = re.search(r"\b(lv_\w+_class)\s*;", line)
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
