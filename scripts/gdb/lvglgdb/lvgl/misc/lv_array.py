from typing import Union

import gdb

from lvglgdb.value import Value, ValueInput


class LVArray(Value):
    """LVGL dynamic array wrapper"""

    def __init__(self, arr: ValueInput, element_type: Union[gdb.Type, str] = None):
        super().__init__(Value.normalize(arr, "lv_array_t"))
        self.element_type = (
            gdb.lookup_type(element_type)
            if isinstance(element_type, str)
            else element_type
        )

    @property
    def size(self) -> int:
        return int(self.super_value("size"))

    @property
    def capacity(self) -> int:
        return int(self.super_value("capacity"))

    @property
    def element_size(self) -> int:
        return int(self.super_value("element_size"))

    def __len__(self) -> int:
        return self.size

    def _element_at(self, data: Value, index: int) -> Value:
        """Get element at index from data pointer."""
        addr = int(data) + index * self.element_size
        val = Value(gdb.Value(addr))
        if self.element_type:
            return val.cast(self.element_type, ptr=True).dereference()
        return val

    def __iter__(self):
        data = self.super_value("data")
        for i in range(self.size):
            yield self._element_at(data, i)

    def __getitem__(self, index):
        if isinstance(index, str):
            return super().__getitem__(index)
        if index < 0 or index >= self.size:
            raise IndexError(f"index {index} out of range [0, {self.size})")
        return self._element_at(self.super_value("data"), index)
