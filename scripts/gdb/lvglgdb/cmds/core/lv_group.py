import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_group import LVGroup
from lvglgdb.lvgl.formatter import print_spec_table


class DumpGroup(gdb.Command):
    """dump all focus groups"""

    def __init__(self):
        super(DumpGroup, self).__init__(
            "dump group", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        groups = curr_inst().groups()
        snaps = LVGroup.snapshots(groups)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVGroup._DISPLAY_SPEC["empty_msg"])
