import gdb

from .lvgl import curr_inst, LVDisplay, LVDrawBuf, LVList, LVObject, dump_style_info
from .gdb_cmds import DumpObj, DumpDisplayBuf, InfoDrawUnit, InfoStyle
from .debugger import Debugger
from .value import Value

__all__ = [
    "curr_inst",
    "LVDisplay",
    "LVDrawBuf",
    "LVList",
    "LVObject",
    "dump_style_info",
    "DumpObj",
    "DumpDisplayBuf",
    "InfoDrawUnit",
    "InfoStyle",
    "Value",
]

# Set pagination off and python print-stack full
gdb.execute("set pagination off")
gdb.write("set pagination off\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\n")

# Debugger
Debugger()

# Dumps
DumpObj()
DumpDisplayBuf()

# Infos
InfoStyle()
InfoDrawUnit()
