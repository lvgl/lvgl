import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.core.lv_group import LVGroup


class DumpGroup(gdb.Command):
    """dump all focus groups"""

    def __init__(self):
        super(DumpGroup, self).__init__(
            "dump group", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        LVGroup.print_entries(curr_inst().groups())
