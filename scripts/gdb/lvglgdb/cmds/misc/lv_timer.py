import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_timer import LVTimer


class DumpTimer(gdb.Command):
    """dump all active timers"""

    def __init__(self):
        super(DumpTimer, self).__init__(
            "dump timer", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        LVTimer.print_entries(curr_inst().timers())
