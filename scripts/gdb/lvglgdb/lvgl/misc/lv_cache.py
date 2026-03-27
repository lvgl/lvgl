from typing import Union
import gdb

from lvglgdb.value import Value, ValueInput
from .lv_cache_iter_factory import create_cache_iterator


class LVCache(Value):
    """LVGL cache wrapper - focuses on cache-level operations"""

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Cache Info:"),
            ("Name", "name"),
            ("Node Size", "node_size"),
            ("Max Size", "max_size"),
            ("Current Size", "current_size"),
            ("Free Size", lambda d: d["max_size"] - d["current_size"]),
            ("Enabled", "enabled"),
            ("_skip_if", "iterator_type", None, ("Iterator Type", "iterator_type")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, cache: ValueInput, datatype: Union[gdb.Type, str]):
        super().__init__(Value.normalize(cache, "lv_cache_t"))
        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

    @property
    def name(self) -> str:
        return self.super_value("name").string(fallback="(unnamed)")

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        iter_type = None
        try:
            iterator = create_cache_iterator(self)
            iter_type = iterator.__class__.__name__
        except Exception:
            pass

        d = {
            "addr": hex(int(self)),
            "name": self.name,
            "node_size": int(self.node_size),
            "max_size": int(self.max_size),
            "current_size": int(self.size),
            "enabled": bool(int(self.max_size) > 0),
            "iterator_type": iter_type,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

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

    def sanity_check(self, entry_checker=None):
        """Run sanity check and print results as a table"""
        from lvglgdb.lvgl.formatter import print_table

        iterator = iter(self)
        if iterator is None:
            errors = [f"unsupported cache type: {self.name}"]
        else:
            errors = iterator.sanity_check(entry_checker)

        if errors:
            rows = [{"status": "FAIL", "detail": e} for e in errors]
        else:
            rows = [{"status": "PASS", "detail": f"all {len(iterator)} entries OK"}]

        print_table(rows, ["status", "detail"],
                    lambda i, d: [d["status"], d["detail"]], "",
                    col_align={"detail": "l"})
        return errors


