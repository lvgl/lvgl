from typing import Union
import gdb

from lvglgdb.value import Value, ValueInput


class LVRedBlackTree(Value):
    """LVGL red-black tree iterator"""

    _DISPLAY_SPEC = {
        "info": [
            ("_title", lambda d: "Red-Black Tree Info:"),
            ("Address", "addr"),
            ("Node Size", "size"),
            ("Node Count", "node_count"),
            ("_skip_if", "datatype", None, ("Data Type", "datatype")),
        ],
        "table": [],
        "empty_msg": "",
    }

    def __init__(self, rb: ValueInput, datatype: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(rb, "lv_rb_t"))
        self.lv_rb_node_t = gdb.lookup_type("lv_rb_node_t").pointer()
        self.datatype = (
            gdb.lookup_type(datatype).pointer()
            if isinstance(datatype, str)
            else datatype
        )

    def minimum_from(self, node):
        """Find minimum node from given node"""
        if not node:
            return None

        current = node
        while current.left:
            current = current.left
        return current

    def maximum_from(self, node):
        """Find maximum node from given node"""
        if not node:
            return None

        current = node
        while current.right:
            current = current.right
        return current

    def minimum(self):
        """Find minimum node in the tree"""
        root = self.root
        return self.minimum_from(root)

    def maximum(self):
        """Find maximum node in the tree"""
        root = self.root
        return self.maximum_from(root)

    def __iter__(self):
        """Create a new iterator for this tree"""
        return LVRedBlackTreeIterator(self)

    def __len__(self):
        """Get the number of nodes in the tree"""
        count = 0
        for _ in self:
            count += 1
        return count

    def get_data(self, node):
        """Get typed data from node, with optional type conversion"""
        if not node or not node.data:
            return None

        data = node.data
        if self.datatype:
            return data.cast(self.datatype)
        return data

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        d = {
            "addr": hex(int(self)),
            "size": int(self.size),
            "node_count": len(self),
            "datatype": str(self.datatype) if self.datatype else None,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)


class LVRedBlackTreeIterator:
    """Iterator for LVRedBlackTree that supports multiple traversals"""

    def __init__(self, tree: LVRedBlackTree):
        self.tree = tree
        self.current = tree.minimum()

    def __iter__(self):
        return self

    def __next__(self):
        if not self.current:
            raise StopIteration

        data = self.tree.get_data(self.current)

        # Move to next node (in-order traversal)
        if self.current.right:
            self.current = self.tree.minimum_from(self.current.right)
        else:
            parent = self.current.parent
            while parent and self.current == parent.right:
                self.current = parent
                parent = parent.parent
            self.current = parent

        return data

    def __str__(self):
        """Better string representation for iterator"""
        current = self.current
        if not current:
            return "LVRedBlackTreeIterator(ended)"

        try:
            data = self.tree.get_data(current)
            if data:
                return f"LVRedBlackTreeIterator(current={data})"
        except:
            pass

        return f"LVRedBlackTreeIterator(current=0x{int(current):x})"


