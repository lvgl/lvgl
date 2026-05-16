import gdb
import re


from .lv_cache_iter_base import LVCacheIteratorBase


def create_cache_iterator(cache) -> LVCacheIteratorBase | None:
    """Factory function to create appropriate iterator for cache class"""

    from .lv_cache_lru_rb import LVCacheLRURB

    def get_class(s):
        if s == "lv_cache_class_lru_rb_size":
            return LVCacheLRURB
        elif s == "lv_cache_class_lru_rb_count":
            return LVCacheLRURB
        return None

    try:
        if cache.clz:
            symbol = cache.clz.format_string(symbols=True, address=False)
            m = re.match("<(.*)>", symbol)
            if m:
                symbol = m.group(1)

            cache_class = get_class(symbol)
            if cache_class:
                return iter(cache_class(cache))

    except Exception:
        raise TypeError(f"Unsupported cache type {cache}")
