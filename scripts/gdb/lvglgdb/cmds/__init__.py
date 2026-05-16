import gdb

from .core import DumpObj, DumpIndev, DumpGroup, InfoObjClass, InfoSubject
from .display import DumpDisplayBuf
from .draw import InfoDrawUnit, DumpDrawTask
from .misc import (
    InfoStyle,
    DumpCache,
    CheckPrefix,
    CheckCache,
    DumpAnim,
    DumpTimer,
    DumpImageDecoder,
    DumpFsDrv,
)
from .dashboard import DumpDashboard
from .debugger import Debugger
from .drivers import Lvglobal

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
DumpCache()
CheckPrefix()
CheckCache()
DumpAnim()
DumpTimer()
DumpImageDecoder()
DumpFsDrv()
DumpIndev()
DumpGroup()
DumpDrawTask()

# Infos
InfoStyle()
InfoDrawUnit()
InfoObjClass()
InfoSubject()

# Drivers
Lvglobal()

# Dashboard
DumpDashboard()
