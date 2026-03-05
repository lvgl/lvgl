import gdb
from prettytable import PrettyTable
from lvglgdb.value import Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageCache(object):
    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_cache_data_t")

    def print_info(self):
        self._cache.print_info()

    def print_entries(self):
        """Print image cache entries using prettytable format"""
        iterator = iter(self._cache)
        extra_fields = iterator.extra_fields

        table = PrettyTable()
        fields = (
            ["entry"]
            + extra_fields
            + ["size", "data_size", "cf", "rc", "type", "decoder", "decoded", "src"]
        )
        table.field_names = fields
        table.align = "r"
        table.align["src"] = "l"
        table.align["type"] = "c"

        for entry in iterator:
            entry: LVCacheEntry

            data_ptr = entry.get_data()
            if not data_ptr:
                continue

            ref_cnt = 0
            decoded_ptr = 0
            size_str = ""
            data_size = 0
            cf = 0
            decoder_name = ""
            type_str = "unkn"
            src_str = ""

            try:
                ref_cnt = entry.get_ref_count()
                decoded = data_ptr.decoded
                decoded_ptr = int(decoded) if decoded else 0
                src_type = int(data_ptr.src_type)
                src = data_ptr.src

                header = decoded.header
                w = int(header.w)
                h = int(header.h)
                cf = int(header.cf)
                data_size = int(decoded.data_size) if decoded else 0
                size_str = f"{w}x{h}"

                decoder_name = data_ptr.decoder.name.as_string()

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
                else:
                    src_str = f"{int(src):#x}" if src else "0x0"

            except gdb.error as e:
                src_str = src_str or str(e)

            row = (
                [f"{int(entry):#x}"]
                + iterator.get_extra(entry)
                + [
                    size_str,
                    f"{data_size}",
                    f"{cf}",
                    f"{ref_cnt}",
                    type_str,
                    decoder_name,
                    f"{decoded_ptr:#x}",
                    src_str,
                ]
            )
            table.add_row(row)

        print(table)
