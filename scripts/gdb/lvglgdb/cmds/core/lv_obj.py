import argparse
import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl import LVObject, dump_obj_info
from lvglgdb.value import CorruptedError


class DumpObj(gdb.Command):
    """dump obj tree from specified obj"""

    def __init__(self):
        super(DumpObj, self).__init__(
            "dump obj", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def dump_obj(self, obj: LVObject, depth=0, limit=None):
        if not obj:
            return

        # dump self
        print("  " * depth, end="")
        dump_obj_info(obj)

        if limit is not None and depth >= limit:
            return

        # dump children
        try:
            for child in obj.children:
                self.dump_obj(child, depth + 1, limit=limit)
        except CorruptedError:
            print("  " * (depth + 1) + "(corrupted children)")

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

        if args.root:
            root = gdb.parse_and_eval(args.root)
            root = LVObject(root)
            self.dump_obj(root, limit=args.level)
        else:
            # dump all displays
            depth = 0
            for disp in curr_inst().displays():
                print(f"Display {hex(disp)}")
                try:
                    for screen in disp.screens:
                        print(f"{'  ' * (depth + 1)}Screen@{hex(screen)}")
                        self.dump_obj(screen, depth=depth + 1, limit=args.level)
                except CorruptedError:
                    print(f"{'  ' * (depth + 1)}(corrupted screens)")
