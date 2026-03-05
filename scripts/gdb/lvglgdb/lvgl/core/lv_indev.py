from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput
from .lv_indev_consts import INDEV_TYPE_NAMES


class LVIndev(Value):
    """LVGL input device wrapper"""

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

    @staticmethod
    def print_entries(indevs):
        """Print input devices as a PrettyTable."""
        table = PrettyTable()
        table.field_names = [
            "#",
            "type",
            "enabled",
            "state",
            "read_cb",
            "long_press_time",
            "scroll_limit",
            "group",
        ]
        table.align = "l"

        for i, indev in enumerate(indevs):
            cb_str = indev.read_cb.format_string(symbols=True)
            grp = int(indev.group)
            grp_str = f"0x{grp:x}" if grp else "-"
            table.add_row(
                [
                    i,
                    indev.type_name,
                    indev.enabled,
                    indev.state,
                    cb_str,
                    indev.long_press_time,
                    indev.scroll_limit,
                    grp_str,
                ]
            )

        if not table.rows:
            print("No input devices.")
        else:
            print(table)
