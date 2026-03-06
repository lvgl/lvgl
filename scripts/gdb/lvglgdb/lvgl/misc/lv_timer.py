from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput


class LVTimer(Value):
    """LVGL timer wrapper"""

    def __init__(self, timer: ValueInput):
        super().__init__(Value.normalize(timer, "lv_timer_t"))

    @property
    def period(self) -> int:
        return int(self.super_value("period"))

    @property
    def last_run(self) -> int:
        return int(self.super_value("last_run"))

    @property
    def timer_cb(self) -> Value:
        return self.super_value("timer_cb")

    @property
    def repeat_count(self) -> int:
        return int(self.super_value("repeat_count"))

    @property
    def paused(self) -> bool:
        return bool(int(self.super_value("paused")))

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def auto_delete(self) -> bool:
        return bool(int(self.super_value("auto_delete")))

    @staticmethod
    def print_entries(timers):
        """Print timers as a PrettyTable."""
        table = PrettyTable()
        table.field_names = [
            "#",
            "callback",
            "period",
            "freq",
            "last_run",
            "repeat",
            "paused",
        ]
        table.align = "l"

        for i, timer in enumerate(timers):
            cb_str = timer.timer_cb.format_string(symbols=True, address=True)
            repeat = "inf" if timer.repeat_count == -1 else str(timer.repeat_count)
            freq = f"{1000 / timer.period:.1f}Hz" if timer.period > 0 else "-"
            table.add_row(
                [
                    i,
                    cb_str,
                    timer.period,
                    freq,
                    timer.last_run,
                    repeat,
                    timer.paused,
                ]
            )

        if not table.rows:
            print("No active timers.")
        else:
            print(table)
