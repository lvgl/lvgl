from ..core.lv_obj import LVObject
from ..draw.lv_draw_buf import LVDrawBuf
from lvglgdb.value import Value, ValueInput


class LVDisplay(Value):
    """LVGL display"""

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: f"Display @{d['addr']}"),
            ("hor_res", "hor_res"),
            ("ver_res", "ver_res"),
            ("screen_count", "screen_count"),
        ],
        "table": [],
        "empty_msg": "No displays.",
    }

    def __init__(self, disp: ValueInput):
        super().__init__(Value.normalize(disp, "lv_display_t"))

    @property
    def hor_res(self) -> int:
        """Get horizontal resolution in pixels"""
        return int(self.super_value("hor_res"))

    @property
    def ver_res(self) -> int:
        """Get vertical resolution in pixels"""
        return int(self.super_value("ver_res"))

    @property
    def screens(self):
        screens = self.super_value("screens")
        for i in range(self.screen_cnt):
            yield LVObject(screens[i])

    # Buffer-related properties
    @property
    def buf_1(self):
        """Get first draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_1")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_2(self):
        """Get second draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_2")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_act(self):
        """Get currently active draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_act")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "hor_res": self.hor_res,
            "ver_res": self.ver_res,
            "screen_count": int(self.screen_cnt),
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
