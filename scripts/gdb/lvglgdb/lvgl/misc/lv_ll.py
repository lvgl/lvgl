from typing import Union
import gdb

from lvglgdb.value import Value


class LVList(Value):
    """LVGL linked list iterator"""

    def __init__(self, ll: Value, nodetype: Union[gdb.Type, str] = None):
        if not ll:
            raise ValueError("Invalid linked list")
        super().__init__(ll)

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

        nodetype = self.nodetype if self.nodetype else self.lv_ll_node_t
        node = self.current.cast(nodetype)

        self.current = self._next(self.current)
        return node

    @property
    def len(self):
        len = 0
        node = self.head
        while node:
            len += 1
            node = self._next(node)
        return len
