import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_indev import LVIndev


class DumpIndev(gdb.Command):
    """dump all input devices"""

    def __init__(self):
        super(DumpIndev, self).__init__(
            "dump indev", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        LVIndev.print_entries(curr_inst().indevs())
