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

    def print_info(self):
        print(f"Event: code={self.code_name}({self.code})")
        print(f"  current_target={self.current_target}")
        print(f"  original_target={self.original_target}")
        print(f"  deleted={self.deleted} stop_processing={self.stop_processing}")


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

    def print_info(self):
        cb_str = self.cb.format_string(symbols=True, address=True)
        flags = []
        if self.is_preprocess:
            flags.append("PRE")
        if self.is_marked_deleting:
            flags.append("DEL")
        flag_str = f" [{','.join(flags)}]" if flags else ""
        print(f"  cb={cb_str} filter={self.filter_name}({self.filter_code}){flag_str}")


class LVEventList(Value):
    """LVGL event list wrapper (lv_event_list_t contains lv_array_t)"""

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
    def print_entries(event_dscs):
        """Print event descriptors as a PrettyTable."""
        from prettytable import PrettyTable

        table = PrettyTable()
        table.field_names = ["#", "callback", "filter", "flags", "user_data"]
        table.align = "l"

        for i, dsc in enumerate(event_dscs):
            cb_str = dsc.cb.format_string(symbols=True, address=True)
            flags = []
            if dsc.is_preprocess:
                flags.append("PRE")
            if dsc.is_marked_deleting:
                flags.append("DEL")
            table.add_row(
                [i, cb_str, dsc.filter_name, ",".join(flags) or "-", dsc.user_data]
            )

        if not table.rows:
            print("No event descriptors.")
        else:
            print(table)
