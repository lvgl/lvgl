import gdb
from prettytable import PrettyTable
from lvglgdb.value import Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageHeaderCacheData(Value):
    """Wrapper for lv_image_header_cache_data_t with sanity check"""

    def __init__(self, data_ptr):
        super().__init__(Value.normalize(data_ptr, "lv_image_header_cache_data_t"))

    def sanity_check(self, entry_addr):
        """Validate image header cache data fields"""
        errors = []
        prefix = f"entry {entry_addr:#x}"

        header = self.header
        w = int(header.w)
        h = int(header.h)
        if w <= 0 or h <= 0:
            errors.append(f"{prefix}: invalid size {w}x{h}")

        src_type = int(self.src_type)
        if src_type not in (0, 1):
            errors.append(f"{prefix}: unknown src_type {src_type}")

        src = self.src
        if not src or int(src) == 0:
            errors.append(f"{prefix}: null src pointer")

        return errors


class LVImageHeaderCache(object):
    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_header_cache_data_t")

    def print_info(self):
        self._cache.print_info()

    def print_entries(self):
        """Print image header cache entries using prettytable format"""
        iterator = iter(self._cache)
        extra_fields = iterator.extra_fields

        table = PrettyTable()
        fields = (
            ["entry"] + extra_fields + ["size", "cf", "rc", "type", "decoder", "src"]
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
            size_str = ""
            cf = 0
            decoder_name = ""
            type_str = "unkn"
            src_str = ""

            try:
                ref_cnt = entry.get_ref_count()
                src_type = int(data_ptr.src_type)
                src = data_ptr.src

                header = data_ptr.header
                w = int(header.w)
                h = int(header.h)
                cf = int(header.cf)
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
                    f"{cf}",
                    f"{ref_cnt}",
                    type_str,
                    decoder_name,
                    src_str,
                ]
            )
            table.add_row(row)

        print(table)

    @staticmethod
    def _check_header_entry(entry):
        """Delegate sanity check to LVImageHeaderCacheData"""
        data_ptr = entry.get_data()
        if not data_ptr:
            return [f"entry {int(entry):#x}: null data pointer"]
        try:
            return LVImageHeaderCacheData(data_ptr).sanity_check(int(entry))
        except gdb.error as e:
            return [f"entry {int(entry):#x}: gdb error: {e}"]

    def sanity_check(self):
        """Run sanity check on image header cache with header-specific entry validation"""
        return self._cache.sanity_check(self._check_header_entry)
