import gdb

from typing import Optional, Union


class Value(gdb.Value):
    def __init__(self, value: Union[gdb.Value, 'Value']):
        super().__init__(value)

    def __getitem__(self, key):
        try:
            value = super().__getitem__(key)
        except gdb.error:
            value = super().__getattr__(key)
        return Value(value)

    def __getattr__(self, key):
        if hasattr(super(), key):
            return Value(super().__getattribute__(key))
        return Value(super().__getitem__(key))

    def cast(self, type_name: Union[str, gdb.Type], ptr: bool = False) -> Optional['Value']:
        try:
            gdb_type = gdb.lookup_type(type_name) if isinstance(type_name, str) else type_name
            if ptr:
                gdb_type = gdb_type.pointer()
            return Value(super().cast(gdb_type))
        except gdb.error:
            return None

    def super_value(self, attr: str) -> 'Value':
        return self[attr]
