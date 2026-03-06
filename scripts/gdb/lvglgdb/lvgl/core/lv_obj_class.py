import gdb
from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput
from ..misc.lv_utils import format_coord


class LVObjClass(Value):
    """LVGL object class wrapper"""

    def __init__(self, cls: ValueInput):
        super().__init__(Value.normalize(cls, "lv_obj_class_t"))

    @property
    def name(self) -> str:
        n = self.super_value("name")
        return n.string() if int(n) else "(unnamed)"

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

    @staticmethod
    def print_entries(classes):
        """Print object classes as a PrettyTable."""
        table = PrettyTable()
        table.field_names = [
            "#",
            "name",
            "base",
            "size",
            "editable",
            "group_def",
            "default_size",
            "theme_inh",
        ]
        table.align = "l"

        for i, cls in enumerate(classes):
            base = cls.base_class
            base_name = base.name if base else "-"
            table.add_row(
                [
                    i,
                    cls.name,
                    base_name,
                    cls.instance_size,
                    cls.editable,
                    cls.group_def,
                    f"({format_coord(cls.width_def)}, {format_coord(cls.height_def)})",
                    cls.theme_inheritable,
                ]
            )

        if not table.rows:
            print("No object classes found.")
        else:
            print(table)

    def print_info(self):
        chain = list(self.__iter__())
        names = [c.name for c in chain]
        print(f"ObjClass: {' -> '.join(names)}")
        print(
            f"  size={self.instance_size} editable={self.editable} group_def={self.group_def}"
        )
        w = format_coord(self.width_def)
        h = format_coord(self.height_def)
        print(f"  default_size=({w}, {h}) theme_inheritable={self.theme_inheritable}")
        ctor = int(self.constructor_cb)
        dtor = int(self.destructor_cb)
        evt = int(self.event_cb)
        if ctor:
            print(
                f"  constructor_cb = {self.constructor_cb.format_string(symbols=True)}"
            )
        if dtor:
            print(
                f"  destructor_cb  = {self.destructor_cb.format_string(symbols=True)}"
            )
        if evt:
            print(f"  event_cb       = {self.event_cb.format_string(symbols=True)}")
