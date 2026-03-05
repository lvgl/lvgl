import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_image_decoder import LVImageDecoder


class DumpImageDecoder(gdb.Command):
    """dump all registered image decoders"""

    def __init__(self):
        super(DumpImageDecoder, self).__init__(
            "dump image_decoder", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        LVImageDecoder.print_entries(curr_inst().image_decoders())
