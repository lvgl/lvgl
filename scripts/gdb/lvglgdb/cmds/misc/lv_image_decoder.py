import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.misc.lv_image_decoder import LVImageDecoder
from lvglgdb.lvgl.formatter import print_spec_table


class DumpImageDecoder(gdb.Command):
    """dump all registered image decoders"""

    def __init__(self):
        super(DumpImageDecoder, self).__init__(
            "dump image_decoder", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        decoders = curr_inst().image_decoders()
        snaps = LVImageDecoder.snapshots(decoders)
        if snaps:
            print_spec_table(snaps)
        else:
            print(LVImageDecoder._DISPLAY_SPEC["empty_msg"])
