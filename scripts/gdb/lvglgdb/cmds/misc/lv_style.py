import argparse
import gdb

from lvglgdb.lvgl import LVStyle, dump_obj_styles
from lvglgdb.lvgl.formatter import print_table


def _style_row_fn(_i, d):
    """Format a style property row with optional ANSI color block."""
    value_str = d["value_str"]
    color_rgb = d.get("color_rgb")
    if color_rgb:
        r, g, b = color_rgb["r"], color_rgb["g"], color_rgb["b"]
        value_str = f"{value_str} \033[48;2;{r};{g};{b}m  \033[0m"
    return [d["prop_name"], value_str]


def print_style_props(entries):
    """Print style properties as a 2-column table with optional ANSI color."""
    print_table(
        entries, ["prop", "value"], _style_row_fn,
        "Empty style.", align="l", numbered=False,
    )


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
            print_style_props(LVStyle(style).snapshots())
        else:
            print("Usage: info style <style_var> or info style --obj <obj_var>")
