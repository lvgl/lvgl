import gdb
from typing import Optional, Union, Iterator, TYPE_CHECKING

from lvglgdb.value import Value
from ..misc.lv_ll import LVList

"""
LVGL global instance

Note:
    lv_global is the global instance of LVGL.
    It will contain other components like display, draw unit, etc.
    So if you want add a new component, please add it in this class.
    And just import your component when you need it. Not expose to the whole file.
"""

__all__ = ["curr_inst"]

"""
Type hint for IDE, just imported at typechecking phase not in runtime.
"""
if TYPE_CHECKING:
    from ..display.lv_display import LVDisplay


class LVGL:
    """LVGL instance"""

    def __init__(self, lv_global: Value):
        self.lv_global = lv_global.cast("lv_global_t", ptr=True)

    def displays(self) -> "Iterator[LVDisplay]":
        ll = self.lv_global.disp_ll
        if not ll:
            return

        from ..display.lv_display import LVDisplay

        for disp in LVList(ll, "lv_display_t"):
            yield LVDisplay(disp)

    def disp_default(self):
        from ..display.lv_display import LVDisplay

        disp_default = self.lv_global.disp_default
        return LVDisplay(disp_default) if disp_default else None

    def screen_active(self):
        disp = self.lv_global.disp_default
        return disp.act_scr if disp else None

    def draw_units(self):
        unit = self.lv_global.draw_info.unit_head

        # Iterate through all draw units
        while unit:
            yield unit
            unit = unit.next

    def image_cache(self):
        from ..misc.lv_image_cache import LVImageCache

        return LVImageCache(self.lv_global.img_cache)

    def image_header_cache(self):
        from ..misc.lv_image_header_cache import LVImageHeaderCache

        return LVImageHeaderCache(self.lv_global.img_header_cache)


class _LVGLSingleton:
    __slots__ = ("_lvgl", "_ready")

    def __init__(self) -> None:
        self._lvgl: Optional[object] = None
        self._ready = False

    def ensure_init(self, lv_global: Union[gdb.Value, Value, None] = None) -> bool:
        if self._ready:
            return True

        if lv_global is None:
            try:
                lv_global = Value(gdb.parse_and_eval("lv_global").address)
            except gdb.error as e:
                print(f"Failed to get lv_global: {e}")
                return False
        elif not isinstance(lv_global, Value):
            lv_global = Value(lv_global).cast("lv_global_t", ptr=True)

        if not lv_global.inited:
            print(
                "\x1b[31mlvgl is not initialized yet. "
                "Please call `ensure_init()` later.\x1b[0m"
            )
            return False

        self._lvgl = LVGL(lv_global)
        self._ready = True
        return True

    def __getattr__(self, name: str):
        if not self._ready and not self.ensure_init():
            raise RuntimeError("LVGL singleton not ready")
        return getattr(self._lvgl, name)

    def reset(self) -> None:
        self._lvgl = None
        self._ready = False


__curr_inst = _LVGLSingleton()


def curr_inst() -> _LVGLSingleton:
    """Get the global instance of LVGL"""
    return __curr_inst
