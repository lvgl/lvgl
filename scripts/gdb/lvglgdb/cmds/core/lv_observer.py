import gdb

from lvglgdb.lvgl.core.lv_observer import LVSubject


class InfoSubject(gdb.Command):
    """show subject and its observers"""

    def __init__(self):
        super(InfoSubject, self).__init__(
            "info subject", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        if not args.strip():
            print("Usage: info subject <expression>")
            return
        try:
            subject = LVSubject(args.strip())
        except gdb.error as e:
            print(f"Error: {e}")
            return
        subject.print_info()
