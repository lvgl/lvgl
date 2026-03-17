from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput


class LVDrawUnit(Value):
    """LVGL draw unit wrapper"""

    def __init__(self, unit: ValueInput):
        super().__init__(Value.normalize(unit, "lv_draw_unit_t"))

    @property
    def name(self) -> str:
        n = self.super_value("name")
        return n.string() if int(n) else "(unnamed)"

    @property
    def idx(self) -> int:
        return int(self.super_value("idx"))

    @property
    def next(self):
        n = self.super_value("next")
        return LVDrawUnit(n) if int(n) else None

    def __iter__(self):
        node = self
        while node:
            yield node
            node = node.next

    @staticmethod
    def print_entries(units):
        """Print draw units as a PrettyTable."""
        table = PrettyTable()
        table.field_names = ["#", "name", "idx"]
        table.align = "l"

        for i, unit in enumerate(units):
            table.add_row([i, unit.name, unit.idx])

        if not table.rows:
            print("No draw units.")
        else:
            print(table)
