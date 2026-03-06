from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput


def _fmt_cb(cb: Value) -> str:
    """Format a callback pointer as symbol or hex."""
    addr = int(cb)
    if not addr:
        return "-"
    return cb.format_string(symbols=True, address=True)


class LVAnim(Value):
    """LVGL animation wrapper"""

    def __init__(self, anim: ValueInput):
        super().__init__(Value.normalize(anim, "lv_anim_t"))

    @property
    def var(self) -> Value:
        return self.super_value("var")

    @property
    def exec_cb(self) -> Value:
        return self.super_value("exec_cb")

    @property
    def start_cb(self) -> Value:
        return self.super_value("start_cb")

    @property
    def completed_cb(self) -> Value:
        return self.super_value("completed_cb")

    @property
    def deleted_cb(self) -> Value:
        return self.super_value("deleted_cb")

    @property
    def path_cb(self) -> Value:
        return self.super_value("path_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def start_value(self) -> int:
        return int(self.super_value("start_value"))

    @property
    def current_value(self) -> int:
        return int(self.super_value("current_value"))

    @property
    def end_value(self) -> int:
        return int(self.super_value("end_value"))

    @property
    def duration(self) -> int:
        return int(self.super_value("duration"))

    @property
    def act_time(self) -> int:
        return int(self.super_value("act_time"))

    @property
    def reverse_duration(self) -> int:
        return int(self.super_value("reverse_duration"))

    @property
    def reverse_delay(self) -> int:
        return int(self.super_value("reverse_delay"))

    @property
    def repeat_delay(self) -> int:
        return int(self.super_value("repeat_delay"))

    @property
    def repeat_cnt(self) -> int:
        return int(self.super_value("repeat_cnt"))

    @property
    def is_paused(self) -> bool:
        return bool(int(self.super_value("is_paused")))

    @property
    def reverse_play_in_progress(self) -> bool:
        return bool(int(self.super_value("reverse_play_in_progress")))

    @property
    def early_apply(self) -> bool:
        return bool(int(self.super_value("early_apply")))

    def _status_str(self) -> str:
        """Short status string for table display."""
        if self.is_paused:
            return "paused"
        if self.reverse_play_in_progress:
            return "reverse"
        return "running"

    def print_info(self):
        """Print detailed info for a single animation."""
        print(f"Animation @{hex(int(self))}")
        print(f"  var          = {self.var}")
        print(f"  exec_cb      = {_fmt_cb(self.exec_cb)}")
        print(f"  path_cb      = {_fmt_cb(self.path_cb)}")
        print(f"  start_cb     = {_fmt_cb(self.start_cb)}")
        print(f"  completed_cb = {_fmt_cb(self.completed_cb)}")
        print(f"  deleted_cb   = {_fmt_cb(self.deleted_cb)}")
        print(f"  user_data    = {self.user_data}")
        print(
            f"  value        = {self.start_value} -> {self.current_value} -> {self.end_value}"
        )
        print(f"  duration     = {self.duration}ms  act_time={self.act_time}ms")
        repeat = "inf" if self.repeat_cnt == 0xFFFFFFFF else str(self.repeat_cnt)
        print(f"  repeat       = {repeat}  repeat_delay={self.repeat_delay}ms")
        print(
            f"  reverse      = dur={self.reverse_duration}ms  delay={self.reverse_delay}ms"
        )
        print(f"  status       = {self._status_str()}  early_apply={self.early_apply}")

    @staticmethod
    def print_entries(anims):
        """Print animations as a PrettyTable."""
        table = PrettyTable()
        table.field_names = [
            "#",
            "var",
            "exec_cb",
            "value(start/cur/end)",
            "duration",
            "act_time",
            "repeat",
            "status",
        ]
        table.align = "l"

        for i, anim in enumerate(anims):
            cb_str = _fmt_cb(anim.exec_cb)
            repeat = "inf" if anim.repeat_cnt == 0xFFFFFFFF else str(anim.repeat_cnt)
            value_str = f"{anim.start_value}/{anim.current_value}/{anim.end_value}"
            table.add_row(
                [
                    i,
                    anim.var,
                    cb_str,
                    value_str,
                    f"{anim.duration}ms",
                    f"{anim.act_time}ms",
                    repeat,
                    anim._status_str(),
                ]
            )

        if not table.rows:
            print("No active animations.")
        else:
            print(table)
