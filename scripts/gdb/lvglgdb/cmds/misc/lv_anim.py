import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_anim import LVAnim
from lvglgdb.lvgl.formatter import print_info, print_spec_table


class DumpAnim(gdb.Command):
    """dump all active animations"""

    def __init__(self):
        super(DumpAnim, self).__init__(
            "dump anim", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        anims = list(curr_inst().anims())
        if not anims:
            print("No active animations.")
            return

        if args.strip() == "--detail":
            for anim in anims:
                print_info(anim.snapshot())
                print()
        else:
            print_spec_table(LVAnim.snapshots(anims))
