from typing import Union
import gdb

from lvglgdb.value import Value


class LVRedBlackTree(Value):
    """LVGL red-black tree iterator"""

    def __init__(
        self, rb: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str] = None
    ):
        # Convert to Value first if needed
        if isinstance(rb, int):
            rb = Value(rb).cast("lv_rb_t", ptr=True)
            if rb is None:
                raise ValueError("Failed to cast pointer to lv_rb_t")
        elif isinstance(rb, gdb.Value) and not isinstance(rb, Value):
            rb = Value(rb)
        elif not rb:
            raise ValueError("Invalid red-black tree")
        super().__init__(rb)

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

    def format_data(self, data):
        """Format data for display - simple GDB style"""
        if data is None:
            return "None"

        try:
            ptr_addr = f"0x{int(data):x}"
        except:
            return str(data)

        if self.datatype and data:
            try:
                struct_data = data.dereference()
                return f"{ptr_addr} -> {struct_data}"
            except:
                pass

        return ptr_addr

    def print_info(self):
        """Dump basic tree information"""
        print(f"Red-Black Tree Info:")
        print(f"  Size: {int(self.size)}")
        print(f"  Node Count: {len(self)}")
        print(f"  Root: {self.root}")
        if self.root:
            root_color = "Red" if int(self.root.color) == 0 else "Black"
            print(f"  Root Color: {root_color}")
        if self.datatype:
            print(f"  Data Type: {self.datatype}")

    def print_tree(self, max_items=10):
        """Print tree data in a readable format"""
        print(f"Red-Black Tree Contents ({len(self)} total items):")

        count = 0
        for i, data in enumerate(self):
            if count >= max_items:
                print(f"  ... showing first {max_items} of {len(self)} items")
                break

            formatted = self.format_data(data)
            print(f"  [{i}] {formatted}")
            count += 1

        if count == 0:
            print("  (empty)")
        elif count < len(self):
            print(f"  ... {len(self) - count} more items not shown")


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


def dump_rb_info(
    rb: Union[Value, gdb.Value, int], datatype: Union[gdb.Type, str] = None
):
    """Dump red-black tree information"""
    tree = LVRedBlackTree(rb, datatype=datatype)
    tree.print_info()
