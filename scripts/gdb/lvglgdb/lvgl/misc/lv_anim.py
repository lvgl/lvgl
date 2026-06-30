from lvglgdb.value import Value, ValueInput


class LVAnim(Value):
    """LVGL animation wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: f"Animation @{d['addr']}"),
            "var",
            "exec_cb",
            "path_cb",
            "start_cb",
            "completed_cb",
            "deleted_cb",
            "user_data",
            ("value", lambda d: (
                f"{d['start_value']} -> {d['current_value']}"
                f" -> {d['end_value']}"
            )),
            ("duration", lambda d: (
                f"{d['duration']}ms  act_time={d['act_time']}ms"
            )),
            ("repeat", lambda d: (
                f"{'inf' if d['repeat_cnt'] == 0xFFFFFFFF else d['repeat_cnt']}"
                f"  repeat_delay={d['repeat_delay']}ms"
            )),
            ("reverse", lambda d: (
                f"dur={d['reverse_duration']}ms"
                f"  delay={d['reverse_delay']}ms"
            )),
            ("status", lambda d: (
                f"{d['status']}  early_apply={d['early_apply']}"
            )),
        ],
        "table": [
            ("value(start/cur/end)", lambda d: (
                f"{d['start_value']}/{d['current_value']}/{d['end_value']}"
            )),
            ("duration", lambda d: f"{d['duration']}ms"),
            ("act_time", lambda d: f"{d['act_time']}ms"),
            ("repeat", lambda d: (
                "inf" if d["repeat_cnt"] == 0xFFFFFFFF
                else str(d["repeat_cnt"])
            )),
        ],
        "empty_msg": "No active animations.",
    }

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
        return self.safe_field("reverse_duration", 0, int)

    @property
    def reverse_delay(self) -> int:
        return self.safe_field("reverse_delay", 0, int)

    @property
    def repeat_delay(self) -> int:
        return int(self.super_value("repeat_delay"))

    @property
    def repeat_cnt(self) -> int:
        return int(self.super_value("repeat_cnt"))

    @property
    def is_paused(self) -> bool:
        return self.safe_field("is_paused", False, bool)

    @property
    def reverse_play_in_progress(self) -> bool:
        return self.safe_field("reverse_play_in_progress", False, bool)

    @property
    def early_apply(self) -> bool:
        return self.safe_field("early_apply", False, bool)

    def _status_str(self) -> str:
        """Short status string for table display."""
        if self.is_paused:
            return "paused"
        if self.reverse_play_in_progress:
            return "reverse"
        return "running"

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        d = {
            "addr": hex(int(self)),
            "var": str(self.var),
            "var_addr": ptr_or_none(self.var),
            "exec_cb": fmt_cb(self.exec_cb),
            "path_cb": fmt_cb(self.path_cb),
            "start_cb": fmt_cb(self.start_cb),
            "completed_cb": fmt_cb(self.completed_cb),
            "deleted_cb": fmt_cb(self.deleted_cb),
            "user_data": str(self.user_data),
            "start_value": self.start_value,
            "current_value": self.current_value,
            "end_value": self.end_value,
            "duration": self.duration,
            "act_time": self.act_time,
            "repeat_cnt": self.repeat_cnt,
            "repeat_delay": self.repeat_delay,
            "reverse_duration": self.reverse_duration,
            "reverse_delay": self.reverse_delay,
            "status": self._status_str(),
            "early_apply": self.early_apply,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(anims):
        return [anim.snapshot() for anim in anims]
