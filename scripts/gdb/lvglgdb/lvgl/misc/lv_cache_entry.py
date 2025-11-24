from typing import Union
import gdb

from lvglgdb.value import Value


class LVCacheEntry(Value):
    """LVGL cache entry wrapper - focuses on entry-level operations"""

    def __init__(
        self, entry: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str]
    ):
        # Convert to Value first if needed
        if isinstance(entry, int):
            entry = Value(entry).cast("lv_cache_entry_t", ptr=True)
            if entry is None:
                raise ValueError("Failed to cast pointer to lv_cache_entry_t")
        elif isinstance(entry, gdb.Value) and not isinstance(entry, Value):
            entry = Value(entry)
        elif not entry:
            raise ValueError("Invalid cache entry")

        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

        super().__init__(entry)

    @classmethod
    def from_data_ptr(
        cls, data_ptr: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str]
    ):
        """Create LVCacheEntry from data pointer"""

        if data_ptr.type == gdb.lookup_type("void").pointer() and datatype is None:
            raise ValueError("Data pointer is void*, datatype must be provided")

        data_ptr = data_ptr.cast(datatype)

        entry_ptr = int(data_ptr) + data_ptr.type.target().sizeof
        return cls(entry_ptr, datatype)

    def print_info(self):
        """Dump cache entry information"""
        print(f"Cache Entry Info:")
        print(f"  Reference Count: {int(self.ref_cnt)}")
        print(f"  Node Size: {int(self.node_size)}")
        print(f"  Flags: {int(self.flags)}")
        print(f"  Invalid: {self.is_invalid()}")
        print(f"  Disable Delete: {self.is_disabled_delete()}")

        # Try to get cache info if available
        try:
            cache = self.cache
            if cache:
                print(f"  Cache: {cache}")
        except:
            pass

    def get_data(self):
        """Get entry data pointer"""
        data_ptr = Value(int(self) - self.get_node_size()).cast(self.datatype)
        return data_ptr

    def is_invalid(self):
        """Check if entry is invalid"""
        return bool(int(self.flags) & 1)  # LV_CACHE_ENTRY_FLAG_INVALID

    def is_disabled_delete(self):
        """Check if entry has disable delete flag"""
        return bool(int(self.flags) & 2)  # LV_CACHE_ENTRY_FLAG_DISABLE_DELETE

    def get_ref_count(self):
        """Get reference count"""
        return int(self.ref_cnt)

    def get_node_size(self):
        """Get node size"""
        return int(self.node_size)

    def get_flags(self):
        """Get flags"""
        return int(self.flags)

    def __str__(self):
        """Provide better string representation for debugging"""
        try:
            data = self.get_data()
            return f"CacheEntry(ref_cnt={self.get_ref_count()}, valid={not self.is_invalid()}, data={data.dereference()})"
        except gdb.error:
            pass

        return super().__str__()


def dump_cache_entry_info(
    entry: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str]
):
    """Dump cache entry information"""
    entry_obj = LVCacheEntry(entry, datatype)
    entry_obj.print_info()
