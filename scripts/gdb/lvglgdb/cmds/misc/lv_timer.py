import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_timer import LVTimer
from lvglgdb.lvgl.formatter import print_spec_table


class DumpTimer(gdb.Command):
    """dump all active timers"""

    def __init__(self):
        super(DumpTimer, self).__init__(
            "dump timer", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        timers = curr_inst().timers()
        snaps = LVTimer.snapshots(timers)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVTimer._DISPLAY_SPEC["empty_msg"])
