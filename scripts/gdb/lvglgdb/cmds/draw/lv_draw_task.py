import gdb

from lvglgdb.value import Value
from lvglgdb.lvgl.draw.lv_draw_task import LVDrawTask
from lvglgdb.lvgl.formatter import print_spec_table


class DumpDrawTask(gdb.Command):
    """dump draw tasks from a layer"""

    def __init__(self):
        super(DumpDrawTask, self).__init__(
            "dump draw_task", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        if not args.strip():
            print("Usage: dump draw_task <layer_expression>")
            return

        layer = Value(gdb.parse_and_eval(args.strip()))
        task_head = layer.draw_task_head
        if not int(task_head):
            print("No draw tasks on this layer.")
            return
        print_spec_table(
            LVDrawTask.snapshots(LVDrawTask(task_head)),
        )
