import gdb

from lvglgdb.value import Value, ValueInput
from .lv_cache_iter_base import LVCacheIteratorBase
from .lv_cache_entry import LVCacheEntry
from .lv_cache import LVCache
from .lv_ll import LVList


class LVCacheLRURBIterator(LVCacheIteratorBase):
    """Iterator for LRU RB cache implementation - traverses linked list in LRU order"""

    def __init__(self, cache):
        super().__init__(cache)

    def _collect_entries(self):
        """Collect entries from LRU RB cache by traversing the linked list (MRU→LRU order)"""
        try:
            lru_cache = self.cache.cast("lv_lru_rb_t_", ptr=True)
            if not lru_cache:
                return

            rb_size = int(lru_cache.rb.size)
            ptr_size = gdb.lookup_type("void").pointer().sizeof
            rb_node_pp_t = gdb.lookup_type("lv_rb_node_t").pointer().pointer()

            # ll stores lv_rb_node_t* per node
            for ll_node in LVList(lru_cache.ll):
                rb_node = Value(ll_node).cast(rb_node_pp_t)
                data = rb_node.data

                entry = LVCacheEntry.from_data_ptr(data, self.cache.datatype)

                # Back-pointer to ll node: *(void**)(data + rb.size - sizeof(void*))
                entry.extra = (
                    Value(int(data) + rb_size - ptr_size)
                    .cast(gdb.lookup_type("void").pointer().pointer())
                    .dereference()
                )

                self._entries.append(entry)

        except Exception as e:
            print(f"Error in _collect_lru_entries: {e}")
            import traceback

            traceback.print_exc()

    @property
    def extra_fields(self):
        return ["ll"]

    def get_extra(self, entry):
        return [f"{int(entry.extra):#x}" if entry.extra else "N/A"]


class LVCacheLRURB(LVCache):
    """LVGL LRU-based cache using red-black tree iterator"""

    def __init__(self, cache: ValueInput):
        datatype = cache.datatype if isinstance(cache, LVCache) else None
        super().__init__(cache, datatype)
        self.cache_base = Value(self)

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


def dump_lru_rb_cache_info(cache: ValueInput):
    """Dump LRU RB cache information"""
    cache_obj = LVCacheLRURB(cache)
    cache_obj.print_info()
