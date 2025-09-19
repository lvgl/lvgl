from .value import *
from .lvgl import *
from .lv_global import set_lvgl_instance
from .core.lv_obj import DumpObj
from .display.lv_display import DumpDisplayBuf
from .draw.lv_draw import InfoDrawUnit
from .misc.lv_style import InfoStyle
from .debugger import *

# Debugger
Debugger()

# Dumps
DumpObj()
DumpDisplayBuf()

# Infos
InfoStyle()
InfoDrawUnit()

# Set instance
set_lvgl_instance(None)
