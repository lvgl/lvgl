from lvglgdb.value import Value, ValueInput


class LVImageDecoder(Value):
    """LVGL image decoder wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("name", "name"),
            ("info_cb", "info_cb"),
            ("open_cb", "open_cb"),
            ("close_cb", "close_cb"),
        ],
        "table": [],
        "empty_msg": "No registered image decoders.",
    }

    def __init__(self, decoder: ValueInput):
        super().__init__(Value.normalize(decoder, "lv_image_decoder_t"))

    @property
    def name(self) -> str:
        n = self.super_value("name")
        return n.string() if int(n) else "(unnamed)"

    @property
    def info_cb(self) -> Value:
        return self.super_value("info_cb")

    @property
    def open_cb(self) -> Value:
        return self.super_value("open_cb")

    @property
    def close_cb(self) -> Value:
        return self.super_value("close_cb")

    @property
    def get_area_cb(self) -> Value:
        return self.super_value("get_area_cb")

    @property
    def custom_draw_cb(self) -> Value:
        return self.super_value("custom_draw_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot
        from lvglgdb.lvgl.data_utils import fmt_cb

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "info_cb": fmt_cb(self.info_cb),
            "open_cb": fmt_cb(self.open_cb),
            "close_cb": fmt_cb(self.close_cb),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(decoders):
        return [dec.snapshot() for dec in decoders]
