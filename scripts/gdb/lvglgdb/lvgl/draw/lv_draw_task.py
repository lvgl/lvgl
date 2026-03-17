from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput
from .lv_draw_consts import DRAW_TASK_TYPE_NAMES, DRAW_TASK_STATE_NAMES


class LVDrawTask(Value):
    """LVGL draw task wrapper"""

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
        return (int(a["x1"]), int(a["y1"]), int(a["x2"]), int(a["y2"]))

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

    @staticmethod
    def print_entries(tasks):
        """Print draw tasks as a PrettyTable."""
        table = PrettyTable()
        table.field_names = ["#", "type", "state", "area", "opa", "unit_id"]
        table.align = "l"

        for i, t in enumerate(tasks):
            table.add_row(
                [
                    i,
                    t.type_name,
                    t.state_name,
                    t.area,
                    t.opa,
                    t.preferred_draw_unit_id,
                ]
            )

        if not table.rows:
            print("No draw tasks.")
        else:
            print(table)
