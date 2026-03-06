from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput
from ..misc.lv_ll import LVList


class LVGroup(Value):
    """LVGL focus group wrapper"""

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
        if not int(focus_pp):
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

    def __iter__(self):
        from .lv_obj import LVObject

        for obj_ptr in LVList(self.obj_ll, "lv_obj_t"):
            yield LVObject(obj_ptr)

    @staticmethod
    def print_entries(groups):
        """Print focus groups as a PrettyTable."""
        table = PrettyTable()
        table.field_names = ["#", "objects", "frozen", "editing", "wrap", "focused"]
        table.align = "l"

        for i, group in enumerate(groups):
            focus = group.obj_focus
            if focus:
                from .lv_obj import LVObject

                focus_obj = LVObject(focus)
                focus_str = f"{focus_obj.class_name}@{int(focus):x}"
            else:
                focus_str = "(none)"
            table.add_row(
                [
                    i,
                    group.obj_count,
                    group.frozen,
                    group.editing,
                    group.wrap,
                    focus_str,
                ]
            )

        if not table.rows:
            print("No focus groups.")
        else:
            print(table)
