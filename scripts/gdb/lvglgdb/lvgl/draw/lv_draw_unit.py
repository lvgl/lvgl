from lvglgdb.value import Value, ValueInput


class LVDrawUnit(Value):
    """LVGL draw unit wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("name", "name"),
            ("idx", "idx"),
        ],
        "table": [],
        "empty_msg": "No draw units.",
    }

    def __init__(self, unit: ValueInput):
        super().__init__(Value.normalize(unit, "lv_draw_unit_t"))

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    @property
    def idx(self) -> int:
        return self.safe_field("idx", -1, int)

    @property
    def next(self):
        n = self.super_value("next")
        return LVDrawUnit(n) if int(n) else None

    def __iter__(self):
        node = self
        while node:
            yield node
            node = node.next

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "idx": self.idx,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(units):
        return [unit.snapshot() for unit in units]
