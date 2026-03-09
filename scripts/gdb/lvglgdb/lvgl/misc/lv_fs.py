from lvglgdb.value import Value, ValueInput
from .lv_utils import resolve_source_name, build_global_field_map


class LVFsDrv(Value):
    """LVGL filesystem driver wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("letter", lambda d: f"{d['letter']}:"),
            ("type", "driver_name"),
            ("cache_size", "cache_size"),
            ("open_cb", "open_cb"),
            ("read_cb", "read_cb"),
            ("write_cb", "write_cb"),
            ("close_cb", "close_cb"),
        ],
        "table": [],
        "empty_msg": "No registered filesystem drivers.",
    }

    def __init__(self, drv: ValueInput):
        super().__init__(Value.normalize(drv, "lv_fs_drv_t"))

    @property
    def letter(self) -> str:
        return chr(int(self.super_value("letter")))

    # Lazily built address -> name map shared across all instances
    _addr_map = None

    @property
    def driver_name(self) -> str:
        """Infer driver name from lv_global_t field or open_cb source file."""
        if LVFsDrv._addr_map is None:
            m = build_global_field_map("lv_fs_drv_t")
            LVFsDrv._addr_map = {
                addr: name.replace("_fs_drv", "") for addr, name in m.items()
            }
        # Try lv_global_t field match first (no debug info needed)
        name = LVFsDrv._addr_map.get(int(self), None)
        if name:
            return name
        # Fallback: resolve from open_cb source file (needs debug info)
        return resolve_source_name(int(self.open_cb), prefix="lv_fs_") or "unknown"

    @property
    def cache_size(self) -> int:
        return int(self.super_value("cache_size"))

    @property
    def ready_cb(self) -> Value:
        return self.super_value("ready_cb")

    @property
    def open_cb(self) -> Value:
        return self.super_value("open_cb")

    @property
    def close_cb(self) -> Value:
        return self.super_value("close_cb")

    @property
    def read_cb(self) -> Value:
        return self.super_value("read_cb")

    @property
    def write_cb(self) -> Value:
        return self.super_value("write_cb")

    @property
    def seek_cb(self) -> Value:
        return self.super_value("seek_cb")

    @property
    def tell_cb(self) -> Value:
        return self.super_value("tell_cb")

    @property
    def dir_open_cb(self) -> Value:
        return self.super_value("dir_open_cb")

    @property
    def dir_read_cb(self) -> Value:
        return self.super_value("dir_read_cb")

    @property
    def dir_close_cb(self) -> Value:
        return self.super_value("dir_close_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        d = {
            "addr": hex(int(self)),
            "letter": self.letter,
            "driver_name": self.driver_name,
            "cache_size": self.cache_size,
            "open_cb": fmt_cb(self.open_cb),
            "read_cb": fmt_cb(self.read_cb),
            "write_cb": fmt_cb(self.write_cb),
            "close_cb": fmt_cb(self.close_cb),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(drivers):
        return [drv.snapshot() for drv in drivers]
