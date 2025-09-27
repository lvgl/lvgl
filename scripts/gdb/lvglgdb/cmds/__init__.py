import gdb

from .core import DumpObj
from .display import DumpDisplayBuf
from .draw import InfoDrawUnit
from .misc import InfoStyle
from .debugger import Debugger

__all__ = []

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
