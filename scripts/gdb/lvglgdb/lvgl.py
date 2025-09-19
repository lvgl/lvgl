from typing import Iterator

import gdb

from .display.lv_display import LVDisplay
from .misc.lv_ll import LVList

from .value import Value

gdb.execute("set pagination off")
gdb.write("set pagination off\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\n")


class LVGL:
    """LVGL instance"""

    def __init__(self, lv_global: Value):
        self.lv_global = lv_global.cast("lv_global_t", ptr=True)

    def displays(self) -> Iterator[LVDisplay]:
        ll = self.lv_global.disp_ll
        if not ll:
            return

        for disp in LVList(ll, "lv_display_t"):
            yield LVDisplay(disp)

    def disp_default(self):
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
