import argparse
import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl import LVDrawBuf


class DumpDisplayBuf(gdb.Command):
    """dump display buf to image"""

    def __init__(self):
        super(DumpDisplayBuf, self).__init__(
            "dump display", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump display draw buffer.")
        parser.add_argument(
            "-p",
            "--prefix",
            type=str,
            default="",
            help="prefix of dump file path",
        )
        parser.add_argument(
            "-f",
            "--format",
            type=str,
            choices=["bmp", "png"],
            default=None,
            help="dump file format (bmp or png)",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        display = curr_inst().disp_default()
        if not display:
            print("Error: Invalid display pointer")
            return
        buffers = {
            "buf_1": display.buf_1,
            "buf_2": display.buf_2,
        }

        for buf_name, buf_ptr in buffers.items():
            if buf_ptr is not None:
                draw_buf = LVDrawBuf(buf_ptr)
                filename = f"{args.prefix}{buf_name}.{args.format.lower() if args.format else 'bmp'}"
                draw_buf.data_dump(filename, args.format)
            else:
                print(f"Warning: {buf_name} buffer is None, skipping.")
