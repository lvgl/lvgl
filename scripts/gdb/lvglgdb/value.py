import gdb

from typing import Optional, Union


class Value(gdb.Value):
    def __init__(self, value: Union[gdb.Value, "Value"]):
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

    def cast(
        self, type_name: Union[str, gdb.Type], ptr: bool = False
    ) -> Optional["Value"]:
        try:
            gdb_type = (
                gdb.lookup_type(type_name) if isinstance(type_name, str) else type_name
            )
            if ptr:
                gdb_type = gdb_type.pointer()
            return Value(super().cast(gdb_type))
        except gdb.error:
            return None

    def super_value(self, attr: str) -> "Value":
        return self[attr]

    def as_string(self):
        """Convert to string if possible"""
        try:
            return self.string()
        except gdb.error:
            return str(self)

    def __str__(self):
        """Provide better string representation for debugging"""
        try:
            ptr_val = int(self)
            return f"({self.type})0x{ptr_val:x}"
        except gdb.error:
            pass

        return super().__str__()

    def __repr__(self):
        """Provide detailed representation"""
        try:
            content = self.dereference()
            return f"Value({self.__str__()}: {content})"
        except gdb.error:
            pass

        return f"Value({self.__str__()})"
