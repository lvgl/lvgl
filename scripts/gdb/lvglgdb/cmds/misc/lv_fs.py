import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_fs import LVFsDrv
from lvglgdb.lvgl.formatter import print_spec_table


class DumpFsDrv(gdb.Command):
    """dump all registered filesystem drivers"""

    def __init__(self):
        super(DumpFsDrv, self).__init__(
            "dump fs_drv", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        drivers = curr_inst().fs_drivers()
        snaps = LVFsDrv.snapshots(drivers)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVFsDrv._DISPLAY_SPEC["empty_msg"])
