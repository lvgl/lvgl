from typing import Union
import gdb
from prettytable import PrettyTable
from lvglgdb.value import Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageHeaderCache(object):
    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_header_cache_data_t")

    def print_info(self):
        self._cache.print_info()

    def print_entries(self):
        """Print image cache entries using prettytable format"""
        table = PrettyTable()
        table.field_names = ["size", "cf", "rc", "type", "decoder", "src"]
        table.align = "r"  # Right align all columns by default
        table.align["src"] = "l"  # Left align source column
        table.align["type"] = "c"  # Center align type column

        for entry in self._cache:
            entry: LVCacheEntry

            data_ptr = entry.get_data()
            if not data_ptr:
                continue

            try:
                header = data_ptr.header
                w = int(header.w)
                h = int(header.h)
                cf = int(header.cf)

                src_type = int(data_ptr.src_type)
                src = data_ptr.src
                decoder_name = data_ptr.decoder.name.as_string()

                ref_cnt = entry.get_ref_count()

                size_str = f"{w}x{h}"

                if src_type == 0:  # LV_IMAGE_SRC_VARIABLE
                    src_str = src.format_string(
                        symbols=True, address=True, styling=True
                    )
                    type_str = "var"
                elif src_type == 1:  # LV_IMAGE_SRC_FILE
                    src_str = (
                        src.cast("char", ptr=True).as_string() if src else "(null)"
                    )
                    type_str = "file"
                else:  # Unknown type
                    src_str = f"{int(src):#x}" if src else "0x0"
                    type_str = "unkn"

                table.add_row(
                    [
                        size_str,
                        f"{cf}",
                        f"{ref_cnt}",
                        type_str,
                        decoder_name,
                        src_str,
                    ]
                )

            except gdb.error as e:
                table.add_row(["ERROR", "", "", "", "", str(e)])
                continue

        print(table)
