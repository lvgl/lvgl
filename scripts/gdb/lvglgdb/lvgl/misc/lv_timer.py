from lvglgdb.value import Value, ValueInput


class LVTimer(Value):
    """LVGL timer wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("callback", "timer_cb"),
            ("period", "period"),
            ("freq", "frequency"),
            ("last_run", "last_run"),
            ("repeat", lambda d: (
                "inf" if d["repeat_count"] == -1
                else str(d["repeat_count"])
            )),
            ("paused", "paused"),
        ],
        "table": [],
        "empty_msg": "No active timers.",
    }

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

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        freq = f"{1000 / self.period:.1f}Hz" if self.period > 0 else "-"
        d = {
            "addr": hex(int(self)),
            "timer_cb": fmt_cb(self.timer_cb),
            "period": self.period,
            "frequency": freq,
            "last_run": self.last_run,
            "repeat_count": self.repeat_count,
            "paused": self.paused,
            "user_data": str(self.user_data),
            "user_data_addr": ptr_or_none(self.user_data),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(timers):
        return [timer.snapshot() for timer in timers]
