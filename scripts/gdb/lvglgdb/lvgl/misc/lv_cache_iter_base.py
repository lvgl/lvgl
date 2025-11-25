from typing import List, Optional
import gdb

from lvglgdb.value import Value


class LVCacheIteratorBase:
    """Base class for cache iterators"""

    def __init__(self, cache):
        self.cache = cache
        self._entries: List[Value] = []
        self._current_index = 0
        self._collect_entries()

    def __iter__(self):
        return self

    def __next__(self):
        """Get next cache entry"""
        if self._current_index < len(self._entries):
            entry = self._entries[self._current_index]
            self._current_index += 1
            return entry
        else:
            raise StopIteration

    def __len__(self):
        return len(self._entries)

    def __str__(self):
        return f"{self.__class__.__name__}(cache={self.cache.name}, entries={len(self._entries)})"

    def _collect_entries(self):
        """To be implemented by subclasses"""
        raise NotImplementedError("Subclasses must implement _collect_entries")
