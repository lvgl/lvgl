import argparse
import gdb

from lvglgdb.value import Value
from lvglgdb.lvgl import LVObject
from lvglgdb.lvgl import dump_style_info


class InfoStyle(gdb.Command):
    """dump obj style value for specified obj"""

    def __init__(self):
        super(InfoStyle, self).__init__(
            "info style", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl obj local style.")
        parser.add_argument(
            "obj",
            type=str,
            help="obj to show style.",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        obj = gdb.parse_and_eval(args.obj)
        if not obj:
            print("Invalid obj: ", args.obj)
            return

        obj = Value(obj)

        # show all styles applied to this obj
        for style in LVObject(obj).styles:
            print("  ", end="")
            dump_style_info(style)
