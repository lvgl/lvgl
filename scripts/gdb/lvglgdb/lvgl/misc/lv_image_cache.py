import gdb
from lvglgdb.value import Value
from .lv_cache import LVCache
from .lv_cache_entry import LVCacheEntry


class LVImageCacheData(Value):
    """Wrapper for lv_image_cache_data_t with sanity check"""

    def __init__(self, data_ptr):
        super().__init__(Value.normalize(data_ptr, "lv_image_cache_data_t"))

    def sanity_check(self, entry_addr):
        """Validate image cache data fields"""
        errors = []
        prefix = f"entry {entry_addr:#x}"

        decoded = self.decoded
        if not decoded:
            errors.append(f"{prefix}: null decoded pointer")
            return errors

        header = decoded.header
        w = int(header.w)
        h = int(header.h)
        if w <= 0 or h <= 0:
            errors.append(f"{prefix}: invalid size {w}x{h}")

        data_size = int(decoded.data_size)
        if data_size <= 0:
            errors.append(f"{prefix}: invalid data_size {data_size}")

        src_type = int(self.src_type)
        if src_type not in (0, 1):
            errors.append(f"{prefix}: unknown src_type {src_type}")

        src = self.src
        if not src or int(src) == 0:
            errors.append(f"{prefix}: null src pointer")

        return errors


class LVImageCache(object):
    _DISPLAY_SPEC = {
        "info": [
            ("size", "size"),
            ("data_size", lambda d: str(d["data_size"])),
            ("cf", lambda d: str(d["cf"])),
            ("rc", lambda d: str(d["ref_count"])),
            ("type", "src_type"),
            ("decoder", "decoder_name"),
            ("decoded", "decoded_addr"),
            ("src", "src"),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: Value):
        self._cache = LVCache(cache, "lv_image_cache_data_t")

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

            extras = dict(zip(iterator.extra_fields, iterator.get_extra(entry)))
            d = {
                "entry_addr": f"{int(entry):#x}",
                "extra_fields": extras,
                "size": size_str,
                "data_size": data_size,
                "cf": cf,
                "ref_count": ref_cnt,
                "src_type": type_str,
                "decoder_name": decoder_name,
                "decoded_addr": f"{decoded_ptr:#x}",
                "src": src_str,
            }
            result.append(Snapshot(d, source=entry,
                                   display_spec=self._DISPLAY_SPEC))

        self._last_extra_fields = iterator.extra_fields
        return result

    @staticmethod
    def _check_image_entry(entry):
        """Delegate sanity check to LVImageCacheData"""
        data_ptr = entry.get_data()
        if not data_ptr:
            return [f"entry {int(entry):#x}: null data pointer"]
        try:
            return LVImageCacheData(data_ptr).sanity_check(int(entry))
        except gdb.error as e:
            return [f"entry {int(entry):#x}: gdb error: {e}"]

    def sanity_check(self):
        """Run sanity check on image cache with image-specific entry validation"""
        return self._cache.sanity_check(self._check_image_entry)
