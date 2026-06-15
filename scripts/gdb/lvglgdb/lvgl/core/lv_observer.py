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

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb, ptr_or_none

        d = {
            "addr": hex(int(self)),
            "cb": fmt_cb(self.cb),
            "target": str(self.target),
            "for_obj": self.for_obj,
            "user_data": str(self.user_data),
            "subject_addr": ptr_or_none(self.subject),
            "target_addr": ptr_or_none(self.target),
        }
        return Snapshot(d, source=self)


class LVSubject(Value):
    """LVGL subject wrapper"""

    _OBSERVER_FIELDS = [
        ("_title", lambda d: (
            f"  Observer: cb={d['cb']} target={d['target']}"
            f" for_obj={d['for_obj']}"
        )),
    ]

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: (
                f"Subject: type={d['type_name']}"
                f" subscribers={len(d.get('observers', []))}"
            )),
            ("_children", "observers", _OBSERVER_FIELDS),
        ],
        "table": [],
        "empty_msg": "",
    }

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

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        observers = []
        observer_addrs = []
        for obs in self.__iter__():
            observers.append(obs.snapshot().as_dict())
            observer_addrs.append(hex(int(obs)))

        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "size": self.size,
            "observers": observers,
            "observer_addrs": observer_addrs,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
