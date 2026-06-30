from typing import Union
import gdb

from lvglgdb.value import Value, ValueInput


class LVList(Value):
    """LVGL linked list iterator"""

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Linked List Info:"),
            ("Address", "addr"),
            ("Node Size", "n_size"),
            ("Node Count", "node_count"),
            ("_skip_if", "nodetype", None, ("Node Type", "nodetype")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, ll: ValueInput, nodetype: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(ll, "lv_ll_t"))

        self.nodetype = (
            gdb.lookup_type(nodetype).pointer()
            if isinstance(nodetype, str)
            else nodetype
        )
        self.lv_ll_node_t = gdb.lookup_type("lv_ll_node_t").pointer()
        self.current = self.head
        self._next_offset = self.n_size + self.lv_ll_node_t.sizeof
        self._prev_offset = self.n_size

    def _next(self, node):
        next_value = Value(int(node) + self._next_offset)
        return next_value.cast(self.lv_ll_node_t, ptr=True).dereference()

    def _prev(self, node):
        prev_value = Value(int(node) + self._prev_offset)
        return prev_value.cast(self.lv_ll_node_t, ptr=True).dereference()

    def __iter__(self):
        return self

    def __next__(self):
        if not self.current:
            raise StopIteration

        node = self.current.cast(self.nodetype or self.lv_ll_node_t)
        self.current = self._next(self.current)
        return node

    @property
    def len(self):
        count = 0
        node = self.head
        while node:
            count += 1
            node = self._next(node)
        return count

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "n_size": int(self.n_size),
            "node_count": self.len,
            "nodetype": str(self.nodetype) if self.nodetype else None,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)
