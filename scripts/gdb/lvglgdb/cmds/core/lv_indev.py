import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_indev import LVIndev
from lvglgdb.lvgl.formatter import print_spec_table


class DumpIndev(gdb.Command):
    """dump all input devices"""

    def __init__(self):
        super(DumpIndev, self).__init__(
            "dump indev", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        indevs = curr_inst().indevs()
        snaps = LVIndev.snapshots(indevs)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVIndev._DISPLAY_SPEC["empty_msg"])
