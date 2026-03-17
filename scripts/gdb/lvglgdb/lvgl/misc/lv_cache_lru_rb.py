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

    def _get_lru_params(self):
        """Get common parameters for ll traversal"""
        lru_cache = self.cache.cast("lv_lru_rb_t_", ptr=True)
        if not lru_cache:
            return None
        rb_size = int(lru_cache.rb.size)
        ptr_size = gdb.lookup_type("void").pointer().sizeof
        rb_node_pp_t = gdb.lookup_type("lv_rb_node_t").pointer().pointer()
        void_pp_t = gdb.lookup_type("void").pointer().pointer()
        return lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t

    def _iter_ll_nodes(self, lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t):
        """Yield (ll_addr, data, back_ptr) for each ll node"""
        for ll_node in LVList(lru_cache.ll):
            ll_addr = int(ll_node)
            rb_node = Value(ll_node).cast(rb_node_pp_t)
            data = rb_node.data
            back_ptr = int(
                Value(int(data) + rb_size - ptr_size).cast(void_pp_t).dereference()
            )
            yield ll_addr, data, back_ptr

    def _collect_entries(self):
        """Collect entries from LRU RB cache by traversing the linked list (MRU→LRU order)"""
        try:
            params = self._get_lru_params()
            if not params:
                return
            lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t = params

            for ll_addr, data, back_ptr in self._iter_ll_nodes(
                lru_cache, rb_size, ptr_size, rb_node_pp_t, void_pp_t
            ):
                entry = LVCacheEntry.from_data_ptr(data, self.cache.datatype)
                entry.extra = Value(back_ptr)
                entry.ll_addr = ll_addr
                self._entries.append(entry)

        except Exception as e:
            self._collect_error = f"_collect_entries failed: {e}"
            import traceback

            traceback.print_exc()

    @property
    def extra_fields(self):
        return ["ll"]

    def get_extra(self, entry):
        return [f"{int(entry.extra):#x}"]

    def sanity_check(self, entry_checker=None):
        """Verify rb tree and ll linked list consistency:
        1. Node sets match between rb and ll
        2. Cross-pointers form a closed loop: ll_node→rb_node and rb_node.back_ptr→ll_node
        """
        errors = super().sanity_check(entry_checker)

        try:
            from .lv_rb import LVRedBlackTree

            params = self._get_lru_params()
            if not params:
                errors.append("failed to cast cache to lv_lru_rb_t_")
                return errors
            lru_cache = params[0]

            ll_data_set = set()
            ll_data_list = []
            for entry in self._entries:
                data_addr = int(entry.get_data())
                ll_data_set.add(data_addr)
                ll_data_list.append(data_addr)

                back_ptr = int(entry.extra)
                ll_addr = entry.ll_addr
                if back_ptr != ll_addr:
                    errors.append(
                        f"cross-ptr mismatch: data {data_addr:#x} "
                        f"back_ptr={back_ptr:#x} != ll_node={ll_addr:#x}"
                    )

            rb_data_set = set()
            rb_data_list = []
            rb_tree = LVRedBlackTree(lru_cache.rb)
            for data in rb_tree:
                rb_data_set.add(int(data))
                rb_data_list.append(int(data))

            if len(ll_data_list) != len(ll_data_set):
                from collections import Counter

                for addr, cnt in Counter(ll_data_list).items():
                    if cnt > 1:
                        errors.append(f"duplicate in ll: {addr:#x} appears {cnt} times")

            if len(rb_data_list) != len(rb_data_set):
                from collections import Counter

                for addr, cnt in Counter(rb_data_list).items():
                    if cnt > 1:
                        errors.append(f"duplicate in rb: {addr:#x} appears {cnt} times")

            only_in_ll = ll_data_set - rb_data_set
            only_in_rb = rb_data_set - ll_data_set

            for addr in only_in_ll:
                errors.append(f"node {addr:#x} in ll but not in rb tree")
            for addr in only_in_rb:
                errors.append(f"node {addr:#x} in rb tree but not in ll")

            if not errors:
                ll_count = len(ll_data_set)
                rb_count = len(rb_data_set)
                if ll_count != rb_count:
                    errors.append(f"count mismatch: ll={ll_count}, rb={rb_count}")

        except Exception as e:
            errors.append(f"sanity_check error: {e}")

        return errors


class LVCacheLRURB(LVCache):
    """LVGL LRU-based cache using red-black tree iterator"""

    def __init__(self, cache: ValueInput):
        datatype = cache.datatype if isinstance(cache, LVCache) else None
        super().__init__(cache, datatype)
        self.cache_base = Value(self)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        base = super().snapshot()
        d = base.as_dict()
        d["type"] = "lru_rb"
        return Snapshot(d, source=self,
                       display_spec=getattr(base, "_display_spec", None))

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
