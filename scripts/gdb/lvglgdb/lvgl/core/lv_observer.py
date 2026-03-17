from lvglgdb.value import Value, ValueInput
from ..misc.lv_ll import LVList
from .lv_observer_consts import SUBJECT_TYPE_NAMES


class LVObserver(Value):
    """LVGL observer wrapper"""

    def __init__(self, obs: ValueInput):
        super().__init__(Value.normalize(obs, "lv_observer_t"))

    @property
    def subject(self) -> Value:
        return self.super_value("subject")

    @property
    def cb(self) -> Value:
        return self.super_value("cb")

    @property
    def target(self) -> Value:
        return self.super_value("target")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def auto_free_user_data(self) -> bool:
        return bool(int(self.super_value("auto_free_user_data")))

    @property
    def notified(self) -> bool:
        return bool(int(self.super_value("notified")))

    @property
    def for_obj(self) -> bool:
        return bool(int(self.super_value("for_obj")))

    def print_info(self):
        cb_str = self.cb.format_string(symbols=True, address=True)
        print(
            f"  Observer: cb={cb_str} target={self.target}" f" for_obj={self.for_obj}"
        )


class LVSubject(Value):
    """LVGL subject wrapper"""

    def __init__(self, subject: ValueInput):
        super().__init__(Value.normalize(subject, "lv_subject_t"))

    @property
    def type(self) -> int:
        return int(self.super_value("type"))

    @property
    def type_name(self) -> str:
        return SUBJECT_TYPE_NAMES.get(self.type, f"UNKNOWN({self.type})")

    @property
    def size(self) -> int:
        return int(self.super_value("size"))

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def __iter__(self):
        for obs in LVList(self.subs_ll, "lv_observer_t"):
            yield LVObserver(obs)

    def print_info(self):
        ll = LVList(self.subs_ll, "lv_observer_t")
        print(f"Subject: type={self.type_name} subscribers={ll.len}")
        for obs in self.__iter__():
            obs.print_info()
