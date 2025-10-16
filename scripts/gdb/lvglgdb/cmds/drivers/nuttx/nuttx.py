# GDB script to get lvgl global pointer in NuttX.

import argparse
import sys

import gdb
from nxgdb import utils

# Add current script folder so we can import module lvgl


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

        lv_global = utils.gdb_eval_or_none("lv_global")
        if lv_global:
            gdb.write(f"Found single instance lv_global@{hex(lv_global.address)}\n")
            self.set_lvgl_instance(lv_global)
            return

        # find the lvgl global pointer in tls
        if not args.pid:
            gdb.write("Lvgl is in multi-process mode, need pid argument.\n")
            return
        lv_key = utils.gdb_eval_or_none("lv_nuttx_tlskey") or utils.gdb_eval_or_none(
            "lv_global_default::index"
        )
        if lv_key is None:
            gdb.write("Can't find lvgl tls key in multi-process mode.\n")
            return
        lv_global = utils.get_task_tls(args.pid, lv_key)
        if lv_global:
            gdb.write(f"Found lv_global@{hex(lv_global)}\n")
            self.set_lvgl_instance(lv_global)
        else:
            gdb.write(f"\nCan't find lv_global with tlskey@{hex(lv_key)}.\n")
        gdb.write("\n")
