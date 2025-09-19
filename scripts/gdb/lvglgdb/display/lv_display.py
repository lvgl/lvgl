import argparse
import gdb

from ..core.lv_obj import LVObject
from ..draw.lv_draw_buf import LVDrawBuf
from ..value import Value
from .. import lv_global


class LVDisplay(Value):
    """LVGL display"""

    def __init__(self, disp: Value):
        super().__init__(disp)

    @property
    def hor_res(self) -> int:
        """Get horizontal resolution in pixels"""
        return int(self.super_value("hor_res"))

    @property
    def ver_res(self) -> int:
        """Get vertical resolution in pixels"""
        return int(self.super_value("ver_res"))

    @property
    def screens(self):
        screens = self.super_value("screens")
        for i in range(self.screen_cnt):
            yield LVObject(screens[i])

    # Buffer-related properties
    @property
    def buf_1(self):
        """Get first draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_1")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_2(self):
        """Get second draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_2")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_act(self):
        """Get currently active draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_act")
        return LVDrawBuf(buf_ptr) if buf_ptr else None


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

        display = lv_global.g_lvgl_instance.disp_default()
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
