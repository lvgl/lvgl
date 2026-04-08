import argparse
import json
import time

import gdb

from lvglgdb.lvgl import curr_inst
from .data_collector import collect_all
from .html_renderer import render, render_viewer


class DumpDashboard(gdb.Command):
    """Generate an HTML dashboard of all LVGL runtime state."""

    def __init__(self):
        super().__init__(
            "dump dashboard", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(prog="dump dashboard")
        parser.add_argument("-o", "--output", help="output file path")
        group = parser.add_mutually_exclusive_group()
        group.add_argument(
            "--json", action="store_true", help="output JSON instead of HTML",
        )
        group.add_argument(
            "--viewer", action="store_true",
            help="output empty viewer HTML (no data collection)",
        )

        try:
            opts = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if opts.viewer:
            out = opts.output or "lvgl_viewer.html"
            render_viewer(out)
            gdb.write(f"Viewer written to {out}\n")
            return

        if not curr_inst().ensure_init():
            return

        t0 = time.time()
        data = collect_all()
        elapsed = time.time() - t0

        if opts.json:
            out = opts.output or "lvgl_dashboard.json"
            with open(out, "w", encoding="utf-8") as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        else:
            out = opts.output or "lvgl_dashboard.html"
            render(data, out)

        gdb.write(f"Dashboard written to {out} ({elapsed:.2f}s)\n")
