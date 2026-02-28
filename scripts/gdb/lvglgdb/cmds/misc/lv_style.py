import argparse
import gdb

from lvglgdb.lvgl import LVStyle, dump_obj_styles


class InfoStyle(gdb.Command):
    """Dump style properties. Default: single lv_style_t. Use --obj for object styles."""

    def __init__(self):
        super(InfoStyle, self).__init__(
            "info style", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl style properties.")
        parser.add_argument(
            "style",
            type=str,
            nargs="?",
            help="lv_style_t variable to inspect.",
        )
        parser.add_argument(
            "--obj",
            type=str,
            default=None,
            help="lv_obj_t variable to inspect all styles.",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.obj:
            obj = gdb.parse_and_eval(args.obj)
            if not obj:
                print("Invalid obj:", args.obj)
                return
            dump_obj_styles(obj)
        elif args.style:
            style = gdb.parse_and_eval(args.style)
            if not style:
                print("Invalid style:", args.style)
                return
            LVStyle(style).print_entries()
        else:
            print("Usage: info style <style_var> or info style --obj <obj_var>")
