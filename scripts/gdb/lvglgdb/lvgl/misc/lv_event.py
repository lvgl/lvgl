import gdb

from lvglgdb.value import Value, ValueInput
from .lv_event_consts import EVENT_CODE_NAMES
from .lv_array import LVArray


def event_code_name(code: int) -> str:
    """Map event code integer to human-readable name."""
    return EVENT_CODE_NAMES.get(code, f"UNKNOWN({code})")


class LVEvent(Value):
    """LVGL event wrapper"""

    def __init__(self, event: ValueInput):
        super().__init__(Value.normalize(event, "lv_event_t"))

    @property
    def code(self) -> int:
        return int(self.super_value("code"))

    @property
    def code_name(self) -> str:
        return event_code_name(self.code)

    @property
    def current_target(self) -> Value:
        return self.super_value("current_target")

    @property
    def original_target(self) -> Value:
        return self.super_value("original_target")

    @property
    def param(self) -> Value:
        return self.super_value("param")

    @property
    def prev(self):
        p = self.super_value("prev")
        return LVEvent(p) if int(p) else None

    @property
    def deleted(self) -> bool:
        return bool(int(self.super_value("deleted")))

    @property
    def stop_processing(self) -> bool:
        return bool(int(self.super_value("stop_processing")))



class LVEventDsc(Value):
    """LVGL event descriptor wrapper"""

    def __init__(self, dsc: ValueInput):
        super().__init__(Value.normalize(dsc, "lv_event_dsc_t"))

    @property
    def cb(self) -> Value:
        return self.super_value("cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def filter(self) -> int:
        return int(self.super_value("filter"))

    @property
    def filter_code(self) -> int:
        """Event code with flag bits masked off."""
        return self.filter & ~(0x8000 | 0x10000)

    @property
    def filter_name(self) -> str:
        return event_code_name(self.filter_code)

    @property
    def is_preprocess(self) -> bool:
        return bool(self.filter & 0x8000)

    @property
    def is_marked_deleting(self) -> bool:
        return bool(self.filter & 0x10000)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        flags = []
        if self.is_preprocess:
            flags.append("PRE")
        if self.is_marked_deleting:
            flags.append("DEL")
        d = {
            "cb": fmt_cb(self.cb),
            "filter": self.filter_code,
            "filter_name": self.filter_name,
            "is_preprocess": self.is_preprocess,
            "is_marked_deleting": self.is_marked_deleting,
            "flags_str": ",".join(flags) or "-",
            "user_data": str(self.user_data),
        }
        return Snapshot(d, source=self, display_spec=LVEventList._DISPLAY_SPEC)


class LVEventList(Value):
    """LVGL event list wrapper (lv_event_list_t contains lv_array_t)"""

    _DISPLAY_SPEC = {
        "info": [
            ("callback", "cb"),
            ("filter", "filter_name"),
            ("flags", lambda d: d.get("flags_str", "-")),
            ("user_data", "user_data"),
        ],
        "table": [],
        "empty_msg": "No event descriptors.",
    }

    def __init__(self, event_list: ValueInput):
        super().__init__(Value.normalize(event_list, "lv_event_list_t"))

    @property
    def array(self) -> LVArray:
        """Array stores lv_event_dsc_t* pointers (not inline structs)."""
        dsc_ptr_type = gdb.lookup_type("lv_event_dsc_t").pointer()
        return LVArray(self.super_value("array"), dsc_ptr_type)

    @property
    def is_traversing(self) -> bool:
        return bool(int(self.super_value("is_traversing")))

    @property
    def has_marked_deleting(self) -> bool:
        return bool(int(self.super_value("has_marked_deleting")))

    def __iter__(self):
        """Yield LVEventDsc for each pointer stored in the array."""
        for dsc_ptr in self.array:
            yield LVEventDsc(dsc_ptr)

    def __len__(self) -> int:
        return len(self.array)

    @staticmethod
    def snapshots(event_dscs):
        return [dsc.snapshot() for dsc in event_dscs]
