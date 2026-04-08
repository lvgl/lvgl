from lvglgdb.value import Value, ValueInput
from .lv_indev_consts import INDEV_TYPE_NAMES


class LVIndev(Value):
    """LVGL input device wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("type", "type_name"),
            ("enabled", "enabled"),
            ("state", "state"),
            ("read_cb", "read_cb"),
            ("long_press_time", "long_press_time"),
            ("scroll_limit", "scroll_limit"),
            ("group", "group"),
        ],
        "table": [],
        "empty_msg": "No input devices.",
    }

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

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        grp = int(self.group)
        grp_str = f"0x{grp:x}" if grp else "-"
        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "enabled": self.enabled,
            "state": self.state,
            "read_cb": fmt_cb(self.read_cb),
            "long_press_time": self.long_press_time,
            "scroll_limit": self.scroll_limit,
            "group": grp_str,
            "display_addr": ptr_or_none(self.disp),
            "group_addr": ptr_or_none(self.group),
            "read_timer_addr": ptr_or_none(self.read_timer),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(indevs):
        return [indev.snapshot() for indev in indevs]
