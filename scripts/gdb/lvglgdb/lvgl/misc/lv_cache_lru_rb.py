from typing import Union, List, Optional
import gdb

from lvglgdb.value import Value
from .lv_cache_iter_base import LVCacheIteratorBase
from .lv_rb import LVRedBlackTree
from .lv_cache_entry import LVCacheEntry
from .lv_cache import LVCache


class LVCacheLRURBIterator(LVCacheIteratorBase):
    """Iterator for LRU RB cache implementation - traverses linked list and red-black tree"""

    def __init__(self, cache):
        super().__init__(cache)

    def _collect_entries(self):
        """Collect entries from LRU RB cache by traversing the linked list"""
        try:
            # Cast cache to lv_lru_rb_t_ to access internal structures
            lru_cache = self.cache.cast("lv_lru_rb_t_", ptr=True)
            if not lru_cache:
                return

            # Access the linked list
            rb = lru_cache.rb
            if not rb or not rb.root:
                return

            rb = LVRedBlackTree(rb)
            for node in rb:
                self._entries.append(
                    LVCacheEntry.from_data_ptr(node, self.cache.datatype)
                )
        except Exception as e:
            print(f"Error in _collect_lru_entries: {e}")
            import traceback

            traceback.print_exc()


class LVCacheLRURB(LVCache):
    """LVGL LRU-based cache using red-black tree iterator"""

    def __init__(self, cache: Union[Value, gdb.Value, int]):
        # Convert to Value first if needed
        if isinstance(cache, int):
            cache = Value(cache).cast("lv_cache_lru_rb_t", ptr=True)
            if cache is None:
                raise ValueError("Failed to cast pointer to lv_cache_lru_rb_t")
        elif isinstance(cache, gdb.Value) and not isinstance(cache, Value):
            cache = Value(cache)
        elif not cache:
            raise ValueError("Invalid cache")
        self.cache_base = cache
        super().__init__(cache, cache.datatype)

    def print_info(self):
        """Dump LRU RB cache information"""
        print(f"LRU RB Cache Info:")

        # Try to get cache class info
        try:
            clz = self.clz
            if clz:
                print(f"  Cache Class: {clz}")
                # Check if it's LRU RB based
                if "lru_rb" in str(clz).lower():
                    print(f"  Type: LRU with Red-Black Tree")
        except:
            pass

    def is_count_based(self):
        """Check if this is count-based LRU cache"""
        try:
            name = str(self.name)
            return "count" in name.lower() or "lru_rb_count" in str(self.clz).lower()
        except:
            return False

    def is_size_based(self):
        """Check if this is size-based LRU cache"""
        try:
            name = str(self.name)
            return "size" in name.lower() or "lru_rb_size" in str(self.clz).lower()
        except:
            return False

    def __iter__(self):
        """Create iterator for this LRU RB cache"""
        return LVCacheLRURBIterator(self)

    def items(self):
        """Get all cache entries as a list"""
        entries = []
        for entry in self:
            entries.append(entry)
        return entries


def dump_lru_rb_cache_info(cache: Union[Value, gdb.Value, int]):
    """Dump LRU RB cache information"""
    cache_obj = LVCacheLRURB(cache)
    cache_obj.print_info()
