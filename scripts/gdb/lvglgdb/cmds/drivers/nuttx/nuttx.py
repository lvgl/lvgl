# GDB script to get lvgl global pointer in NuttX.

import argparse
import sys

import gdb


class Lvglobal(gdb.Command):
    """Set which lvgl instance to inspect by finding lv_global pointer in task TLS data."""

    def __init__(self):
        super(Lvglobal, self).__init__("lvglobal", gdb.COMMAND_USER)

    def set_lvgl_instance(self, inst):
        from lvglgdb.lvgl import curr_inst

        curr_inst().reset()
        curr_inst().ensure_init(inst)

    def invoke(self, arg, from_tty):
        parser = argparse.ArgumentParser(description=self.__doc__)
        parser.add_argument("-p", "--pid", type=int, help="Optional process ID")
        try:
            args = parser.parse_args(gdb.string_to_argv(arg))
        except SystemExit:
            return
        try:
            from nxgdb import utils
        except ImportError:
            print("nxgdb is not installed, can't find lvgl global pointer.")
            return

        lv_global = utils.gdb_eval_or_none("lv_global")
        if lv_global:
            print(f"Found single instance lv_global@{hex(lv_global.address)}")
            self.set_lvgl_instance(lv_global)
            return

        # find the lvgl global pointer in tls
        if not args.pid:
            print("LVGL is in multi-process mode; please provide --pid.")
            return
        lv_key = utils.gdb_eval_or_none("lv_nuttx_tlskey")
        if lv_key is None:
            lv_key = utils.gdb_eval_or_none("lv_global_default::index")
        if lv_key is None:
            print("Can't find lvgl tls key in multi-process mode.")
            return
        lv_global = utils.get_task_tls(args.pid, lv_key)
        if lv_global:
            print(f"Found lv_global@{hex(lv_global)}")
            self.set_lvgl_instance(lv_global)
        else:
            print(f"\nCan't find lv_global with tlskey@{hex(lv_key)}.")
