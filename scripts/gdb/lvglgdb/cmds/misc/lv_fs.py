import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_fs import LVFsDrv


class DumpFsDrv(gdb.Command):
    """dump all registered filesystem drivers"""

    def __init__(self):
        super(DumpFsDrv, self).__init__(
            "dump fs_drv", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        LVFsDrv.print_entries(curr_inst().fs_drivers())
