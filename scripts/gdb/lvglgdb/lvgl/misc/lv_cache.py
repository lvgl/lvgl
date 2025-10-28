from typing import Union, List, Optional, Dict
import gdb

from lvglgdb.value import Value
from .lv_cache_iter_factory import create_cache_iterator


class LVCache(Value):
    """LVGL cache wrapper - focuses on cache-level operations"""

    def __init__(
        self, cache: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str]
    ):
        # Convert to Value first if needed
        if isinstance(cache, int):
            cache = Value(cache).cast("lv_cache_t", ptr=True)
            if cache is None:
                raise ValueError("Failed to cast pointer to lv_cache_t")
        elif isinstance(cache, gdb.Value) and not isinstance(cache, Value):
            cache = Value(cache)
        elif not cache:
            raise ValueError("Invalid cache")

        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

        super().__init__(cache)

    def print_info(self):
        """Dump cache information"""
        print(f"Cache Info:")
        print(f"  Name: {self.name.as_string()}")
        print(f"  Node Size: {int(self.node_size)}")
        print(f"  Max Size: {int(self.max_size)}")
        print(f"  Current Size: {int(self.size)}")
        print(f"  Free Size: {int(self.max_size) - int(self.size)}")
        print(f"  Enabled: {bool(int(self.max_size) > 0)}")

        # Try to identify cache type
        try:
            iterator = create_cache_iterator(self)
            print(f"  Iterator Type: {iterator.__class__.__name__}")
            iterator.cache.print_info()
        except gdb.error:
            pass

    def is_enabled(self):
        """Check if cache is enabled"""
        return int(self.max_size) > 0

    def get_usage_percentage(self):
        """Get cache usage percentage"""
        if int(self.max_size) == 0:
            return 0.0
        return int(self.size) / int(self.max_size) * 100.0

    def __iter__(self):
        """Create appropriate iterator based on cache class"""
        return create_cache_iterator(self)

    def items(self):
        """Get all cache entries as a list"""
        entries = []
        for entry in self:
            entries.append(entry)
        return entries

    def print_entries(self, max_entries=10):
        """Print cache entries in readable format"""
        cache_entries = self.items()
        cache_entries_cnt = len(cache_entries)
        print(f"Cache Entries ({cache_entries_cnt} total):")

        count = 0
        for i, entry in enumerate(cache_entries):
            if count >= max_entries:
                print(
                    f"  ... showing first {max_entries} of {cache_entries_cnt} entries"
                )
                break

            print(f"  [{i}] {entry}")
            count += 1

        if count == 0:
            print("  (empty)")
        elif count < int(cache_entries_cnt):
            print(f"  ... {cache_entries_cnt - count} more entries not shown")


def dump_cache_info(
    cache: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str]
):
    """Dump cache information"""
    cache_obj = LVCache(cache, datatype)
    cache_obj.print_info()
