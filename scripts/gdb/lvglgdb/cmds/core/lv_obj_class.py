import argparse

import gdb

from lvglgdb.lvgl.core.lv_obj_class import LVObjClass
from lvglgdb.lvgl.formatter import print_info, print_spec_table


class InfoObjClass(gdb.Command):
    """show object class hierarchy or list all classes"""

    def __init__(self):
        super(InfoObjClass, self).__init__(
            "info obj_class", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Show object class info.")
        parser.add_argument(
            "--all",
            action="store_true",
            default=False,
            help="List all registered object classes.",
        )
        parser.add_argument(
            "expr",
            type=str,
            nargs="?",
            default=None,
            help="Expression evaluating to an lv_obj_class_t.",
        )
        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.all or not args.expr:
            classes = LVObjClass.collect_all()
            snaps = LVObjClass.snapshots(classes)
            if snaps:
                print_spec_table(snaps)
            else:
                print(LVObjClass._DISPLAY_SPEC["empty_msg"])
            return

        try:
            cls = LVObjClass(args.expr)
        except gdb.error as e:
            print(f"Error: {e}")
            return
        print_info(cls.snapshot())
