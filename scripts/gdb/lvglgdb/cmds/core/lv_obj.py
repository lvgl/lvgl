import argparse

import gdb

from lvglgdb.lvgl import LVObject
from .lv_widget import print_all_widgets, print_tree


class DumpObj(gdb.Command):
    """dump obj tree from specified obj"""

    def __init__(self):
        super(DumpObj, self).__init__(
            "dump obj", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl obj tree.")
        parser.add_argument(
            "-L",
            "--level",
            type=int,
            default=None,
            help="Limit the depth of the tree.",
        )
        parser.add_argument(
            "root",
            type=str,
            nargs="?",
            default=None,
            help="Optional root obj to dump.",
        )
        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        # The listing is `info widget`'s, so both commands print the same tree
        # and there is only one renderer to keep current with the widget data.
        if args.root:
            try:
                root = LVObject(gdb.parse_and_eval(args.root))
            except gdb.error as e:
                print(f"Error: {e}")
                return
            print_tree(root, limit=args.level)
        else:
            print_all_widgets(limit=args.level)
