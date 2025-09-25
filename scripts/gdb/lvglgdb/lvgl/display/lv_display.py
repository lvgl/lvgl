from ..core.lv_obj import LVObject
from ..draw.lv_draw_buf import LVDrawBuf
from lvglgdb.value import Value


class LVDisplay(Value):
    """LVGL display"""

    def __init__(self, disp: Value):
        super().__init__(disp)

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
