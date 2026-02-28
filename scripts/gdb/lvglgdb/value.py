import gdb

from typing import Optional, Union


class Value(gdb.Value):
    def __init__(self, value: Union[gdb.Value, "Value"]):
        super().__init__(value)

    @staticmethod
    def normalize(val: "ValueInput", target_type: Optional[str] = None) -> "Value":
        """Normalize input to a typed Value pointer.

        Args:
            val: Input value - int (address), gdb.Value, or Value instance
            target_type: C type name (e.g. "lv_obj_t"). If provided,
                         result is cast to target_type*

        Returns:
            Value instance, optionally cast to target_type*

        Raises:
            ValueError: If target_type lookup fails or cast fails
        """
        if isinstance(val, int):
            val = gdb.Value(val)

        if not isinstance(val, Value):
            val = Value(val)

        if target_type is not None:
            try:
                gdb.lookup_type(target_type)
            except gdb.error:
                raise ValueError(f"Type not found: {target_type}")

            typ = val.type.strip_typedefs()
            if typ.code != gdb.TYPE_CODE_PTR:
                if typ.code in (gdb.TYPE_CODE_INT, gdb.TYPE_CODE_ENUM):
                    val = Value(val.cast(gdb.lookup_type(target_type).pointer()))
                else:
                    val = Value(val.address)

            result = val.cast(target_type, ptr=True)
            if result is None:
                raise ValueError(f"Failed to cast to {target_type}*")
            val = result

        return val

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


# Type alias for all wrapper class __init__ parameters
ValueInput = Union[int, gdb.Value, Value]
