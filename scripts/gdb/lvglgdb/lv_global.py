from typing import Union
import gdb

from .value import Value

g_lvgl_instance = None


def set_lvgl_instance(lv_global: Union[gdb.Value, Value, None]):
    global g_lvgl_instance

    if not lv_global:
        try:
            lv_global = Value(gdb.parse_and_eval("lv_global").address)
        except gdb.error as e:
            print(f"Failed to get lv_global: {e}")
            return

    if not isinstance(lv_global, Value):
        lv_global = Value(lv_global)

    inited = lv_global.inited
    if not inited:
        print(
            "\x1b[31mlvgl is not initialized yet. Please call `set_lvgl_instance(None)` later.\x1b[0m"
        )
        return

    from .lvgl import LVGL
    g_lvgl_instance = LVGL(lv_global)
