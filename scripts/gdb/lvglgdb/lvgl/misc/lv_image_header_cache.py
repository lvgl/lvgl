from lvglgdb.value import CorruptedError, Value
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
    _DISPLAY_SPEC = {
        "info": [
            ("size", "size"),
            ("cf", lambda d: str(d["cf"])),
            ("rc", lambda d: str(d["ref_count"])),
            ("type", "src_type"),
            ("decoder", "decoder_name"),
            ("src", "src"),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_header_cache_data_t")

    def snapshot(self):
        return self._cache.snapshot()

    def snapshots(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        iterator = iter(self._cache)
        result = []

        for entry in iterator:
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

                decoder_name = data_ptr.decoder.name.string()

                if src_type == 0:  # LV_IMAGE_SRC_VARIABLE
                    src_str = src.format_string(
                        symbols=True, address=True, styling=True
                    )
                    type_str = "var"
                elif src_type == 1:  # LV_IMAGE_SRC_FILE
                    src_str = (
                        src.cast("char", ptr=True).string() if src else "(null)"
                    )
                    type_str = "file"
                else:
                    src_str = f"{int(src):#x}" if src else "0x0"

            except CorruptedError as e:
                src_str = src_str or str(e)

            extras = dict(zip(iterator.extra_fields, iterator.get_extra(entry)))
            d = {
                "entry_addr": f"{int(entry):#x}",
                "extra_fields": extras,
                "size": size_str,
                "cf": cf,
                "ref_count": ref_cnt,
                "src_type": type_str,
                "decoder_name": decoder_name,
                "src": src_str,
            }
            result.append(Snapshot(d, source=entry,
                                   display_spec=self._DISPLAY_SPEC))

        self._last_extra_fields = iterator.extra_fields
        return result

    @staticmethod
    def _check_header_entry(entry):
        """Delegate sanity check to LVImageHeaderCacheData"""
        data_ptr = entry.get_data()
        if not data_ptr:
            return [f"entry {int(entry):#x}: null data pointer"]
        try:
            return LVImageHeaderCacheData(data_ptr).sanity_check(int(entry))
        except Exception as e:
            return [f"entry {int(entry):#x}: error: {e}"]

    def sanity_check(self):
        """Run sanity check on image header cache with header-specific entry validation"""
        return self._cache.sanity_check(self._check_header_entry)
