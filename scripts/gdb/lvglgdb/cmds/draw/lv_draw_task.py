import gdb

from lvglgdb.value import Value
from lvglgdb.lvgl.draw.lv_draw_task import LVDrawTask


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
        try:
            val = gdb.parse_and_eval(args.strip())
        except gdb.error as e:
            print(f"Error: {e}")
            return

        LVDrawTask.print_entries(LVDrawTask(Value(val)))
