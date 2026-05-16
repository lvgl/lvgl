from lvglgdb.value import Value, ValueInput
from .lv_draw_consts import DRAW_TASK_TYPE_NAMES, DRAW_TASK_STATE_NAMES


class LVDrawTask(Value):
    """LVGL draw task wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("type", "type_name"),
            ("state", "state_name"),
            ("area", lambda d: (
                f"({d['area']['x1']}, {d['area']['y1']}, "
                f"{d['area']['x2']}, {d['area']['y2']})"
            )),
            ("opa", "opa"),
            ("unit_id", "preferred_draw_unit_id"),
        ],
        "table": [],
        "empty_msg": "No draw tasks.",
    }

    def __init__(self, task: ValueInput):
        super().__init__(Value.normalize(task, "lv_draw_task_t"))

    @property
    def type(self) -> int:
        return int(self.super_value("type"))

    @property
    def type_name(self) -> str:
        return DRAW_TASK_TYPE_NAMES.get(self.type, f"UNKNOWN({self.type})")

    @property
    def state(self) -> int:
        return int(self.super_value("state"))

    @property
    def state_name(self) -> str:
        return DRAW_TASK_STATE_NAMES.get(self.state, f"UNKNOWN({self.state})")

    @property
    def area(self) -> tuple:
        a = self.super_value("area")
        return (int(a.x1), int(a.y1), int(a.x2), int(a.y2))

    @property
    def opa(self) -> int:
        return int(self.super_value("opa"))

    @property
    def next(self):
        n = self.super_value("next")
        return LVDrawTask(n) if int(n) else None

    def __iter__(self):
        node = self
        while node:
            yield node
            node = node.next

    @property
    def preferred_draw_unit_id(self) -> int:
        return int(self.super_value("preferred_draw_unit_id"))

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        x1, y1, x2, y2 = self.area
        d = {
            "addr": hex(int(self)),
            "type": self.type,
            "type_name": self.type_name,
            "state": self.state,
            "state_name": self.state_name,
            "area": {"x1": x1, "y1": y1, "x2": x2, "y2": y2},
            "opa": self.opa,
            "preferred_draw_unit_id": self.preferred_draw_unit_id,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(tasks):
        return [t.snapshot() for t in tasks]
