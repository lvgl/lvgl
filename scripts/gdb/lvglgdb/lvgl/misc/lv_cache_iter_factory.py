import gdb
import re


from .lv_cache_iter_base import LVCacheIteratorBase


def ptr_to_symbol(ptr_val):
    if ptr_val.type.code != gdb.TYPE_CODE_PTR:
        return None
    addr = int(ptr_val)
    out = gdb.execute(f"info symbol {addr:#x}", to_string=True)
    m = re.match(r"^(\S+) in section", out)
    return m.group(1) if m else None


def create_cache_iterator(cache) -> LVCacheIteratorBase | None:
    """Factory function to create appropriate iterator for cache class"""

    from .lv_cache_lru_rb import LVCacheLRURB

    class_mapping = {
        "lv_cache_class_lru_rb_size": LVCacheLRURB,
        "lv_cache_class_lru_rb_count": LVCacheLRURB,
    }

    try:
        if cache.clz:
            symbol = ptr_to_symbol(cache.clz)
            cache_class = class_mapping.get(symbol)
            if cache_class:
                return iter(cache_class(cache))

    except Exception:
        raise TypeError(f"Unsupported cache type {cache}")
