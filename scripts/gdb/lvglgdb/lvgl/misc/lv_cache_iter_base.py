from typing import List, Optional
import gdb

from lvglgdb.value import Value


class LVCacheIteratorBase:
    """Base class for cache iterators"""

    def __init__(self, cache):
        self.cache = cache
        self._entries: List[Value] = []
        self._current_index = 0
        self._collect_error: Optional[str] = None
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

    @property
    def extra_fields(self):
        """Extra column names provided by this iterator, override in subclasses"""
        return []

    def get_extra(self, entry):
        """Get extra column values for an entry, override in subclasses"""
        return []

    def sanity_check(self, entry_checker=None):
        """Run sanity check on cache entries, override in subclasses for structural checks.
        Args:
            entry_checker: optional callable(entry) -> list of error strings
        Returns:
            list of error strings, empty means all good
        """
        self._entries.clear()
        self._current_index = 0
        self._collect_error = None
        self._collect_entries()

        errors = []
        if self._collect_error:
            errors.append(self._collect_error)
        if entry_checker:
            for entry in self._entries:
                try:
                    errs = entry_checker(entry)
                    if errs:
                        errors.extend(errs)
                except Exception as e:
                    errors.append(f"entry_checker raised: {e}")
        return errors
