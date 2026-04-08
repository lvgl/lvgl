import gdb

from typing import Optional, Union


class Value(gdb.Value):
    def __init__(self, value: Union[gdb.Value, "Value"]):
        super().__init__(value)

    @property
    def is_ok(self) -> bool:
        """True for valid Value, False for CorruptedValue."""
        return True

    @staticmethod
    def normalize(val: "ValueInput", target_type: Optional[str] = None) -> "Value":
        """Normalize input to a typed Value pointer.

        Args:
            val: Input value - str (GDB expression), int (address),
                 gdb.Value, or Value instance
            target_type: C type name (e.g. "lv_obj_t"). If provided,
                         result is cast to target_type*

        Returns:
            Value instance, optionally cast to target_type*

        Raises:
            ValueError: If target_type lookup fails or cast fails
            CorruptedError: If val is a CorruptedValue sentinel
        """
        if isinstance(val, CorruptedValue):
            raise CorruptedError(val._addr, val._error)

        if isinstance(val, str):
            val = gdb.parse_and_eval(val)

        if isinstance(val, int):
            val = gdb.Value(val)

        if not isinstance(val, Value):
            val = Value(val)

        # Auto-infer target_type from pointer type when not specified
        if target_type is None:
            typ = val.type.strip_typedefs()
            if typ.code == gdb.TYPE_CODE_PTR:
                target = typ.target().strip_typedefs()
                if target.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
                    # Prefer .name or .tag over str() to avoid polluted
                    # anonymous struct expansions like "const struct {...}".
                    target_type = target.name or target.tag

        if target_type is not None:
            # If val is already a pointer whose target matches target_type,
            # keep the original type to avoid gdb.lookup_type() which may
            # resolve to a different compilation unit's definition when
            # macro-controlled fields cause multiple struct definitions.
            typ = val.type.strip_typedefs()
            if typ.code == gdb.TYPE_CODE_PTR:
                target = typ.target()
                tname = target.name or target.tag or ""
                if tname == target_type:
                    return val

            try:
                gdb.lookup_type(target_type)
            except gdb.error:
                raise ValueError(f"Type not found: {target_type}")

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

    # -- Memory error helpers --

    def _safe_addr(self) -> int:
        """Extract address for diagnostic purposes, returning 0 on failure."""
        try:
            return int(self)
        except (gdb.error, OverflowError):
            return 0

    def _is_memory_error(self, e: gdb.error) -> bool:
        """Check if a gdb.error is actually a memory access failure."""
        msg = str(e).lower()
        return any(k in msg for k in ("memory", "access", "cannot read"))

    def _wrap_memory_error(self, fn) -> Union["Value", "CorruptedValue"]:
        """Call fn, converting memory errors to CorruptedValue."""
        try:
            return fn()
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise

    def read_value(self) -> Union["Value", "CorruptedValue"]:
        """Force a memory read probe. Returns CorruptedValue if unreadable."""
        try:
            addr = int(gdb.Value.__int__(self))
        except (gdb.MemoryError, gdb.error):
            return CorruptedValue(self._safe_addr(), gdb.MemoryError(
                f"Cannot access memory at address {hex(self._safe_addr())}"))
        if addr:
            try:
                gdb.selected_inferior().read_memory(addr, 1)
            except gdb.MemoryError as e:
                return CorruptedValue(addr, e)
            except gdb.error as e:
                if self._is_memory_error(e):
                    return CorruptedValue(addr, e)
                raise
        return self

    # -- Hooked GDB access methods --

    def __bool__(self) -> bool:
        """Wrap gdb.Value truthiness check to handle memory errors."""
        try:
            return bool(gdb.Value.__bool__(self))
        except gdb.MemoryError:
            return False
        except gdb.error as e:
            if self._is_memory_error(e):
                return False
            msg = str(e).lower()
            if "cannot convert" in msg:
                return False
            raise

    def __int__(self) -> int:
        """Wrap gdb.Value int conversion to handle memory errors."""
        try:
            return int(gdb.Value.__int__(self))
        except gdb.MemoryError:
            return 0
        except gdb.error as e:
            if self._is_memory_error(e):
                return 0
            # "Cannot convert value to long" happens when a corrupted
            # pointer dereferences into a struct that GDB cannot coerce
            # to an integer.  Treat as zero rather than propagating.
            msg = str(e).lower()
            if "cannot convert" in msg:
                return 0
            raise

    def __getitem__(self, key):
        try:
            raw = gdb.Value.__getitem__(self, key)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def __getattr__(self, key):
        try:
            if hasattr(gdb.Value, key):
                raw = gdb.Value.__getattribute__(self, key)
            else:
                raw = gdb.Value.__getitem__(self, key)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def dereference(self) -> Union["Value", "CorruptedValue"]:
        try:
            raw = gdb.Value.dereference(self)
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            raise
        return Value(raw)

    def cast(
        self, type_name: Union[str, gdb.Type], ptr: bool = False
    ) -> Optional[Union["Value", "CorruptedValue"]]:
        try:
            gdb_type = (
                gdb.lookup_type(type_name) if isinstance(type_name, str) else type_name
            )
            if ptr:
                gdb_type = gdb_type.pointer()
            return Value(gdb.Value.cast(self, gdb_type))
        except gdb.MemoryError as e:
            return CorruptedValue(self._safe_addr(), e)
        except gdb.error as e:
            if self._is_memory_error(e):
                return CorruptedValue(self._safe_addr(), e)
            return None  # Non-memory error preserves original behavior

    def super_value(self, attr: str) -> "Value":
        return self[attr]

    def safe_field(self, attr: str, default=None, cast=None):
        """Access a struct field that may not exist in older LVGL versions.

        Returns the field value (optionally converted by *cast*), or
        *default* when the field is missing.
        """
        try:
            v = Value(gdb.Value.__getitem__(self, attr))
            return cast(int(v)) if cast is not None else v
        except (gdb.error, gdb.MemoryError):
            return default

    def string(self, *args, fallback=None, **kwargs) -> str:
        """Read a C string, returning fallback on memory error."""
        try:
            return gdb.Value.string(self, *args, **kwargs)
        except gdb.MemoryError:
            return fallback if fallback is not None else f"(corrupted@{hex(self._safe_addr())})"
        except gdb.error as e:
            if self._is_memory_error(e):
                return fallback if fallback is not None else f"(corrupted@{hex(self._safe_addr())})"
            raise

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
            if isinstance(content, CorruptedValue):
                return f"Value({self.__str__()}: {content!r})"
            return f"Value({self.__str__()}: {content})"
        except gdb.error:
            pass

        return f"Value({self.__str__()})"


class CorruptedError(Exception):
    """Raised when a CorruptedValue field is accessed (terminal operation)."""

    def __init__(self, addr: int, original_error: Exception):
        self.addr = addr
        self.original_error = original_error
        super().__init__(
            f"Corrupted value @{hex(addr)}: {original_error}"
        )


class CorruptedValue:
    """Sentinel for inaccessible memory. Infectious through chained ops.

    Not a subclass of Value or gdb.Value. Propagates through dereference()
    and cast(), terminates with CorruptedError on field access.
    bool(CorruptedValue) == False enables natural loop termination.
    """

    def __init__(self, addr: int, error: Exception):
        object.__setattr__(self, "_addr", addr)
        object.__setattr__(self, "_error", error)

    # -- Safe exits: never raise --

    @property
    def is_ok(self) -> bool:
        """Always False for CorruptedValue."""
        return False

    def __int__(self) -> int:
        return self._addr

    def __bool__(self) -> bool:
        return False

    def snapshot(self):
        """Return a corrupted Snapshot with diagnostic info."""
        from .lvgl.snapshot import Snapshot
        d = {
            "addr": hex(self._addr),
            "class_name": "(corrupted)",
            "error": str(self._error),
        }
        return Snapshot(d, source=self)

    def __repr__(self) -> str:
        return f"CorruptedValue(@{hex(self._addr)}: {self._error})"

    def __str__(self) -> str:
        return f"(corrupted@{hex(self._addr)})"

    # -- Propagating ops: return self, never raise --

    def read_value(self):
        return self

    def dereference(self):
        return self

    def cast(self, *args, **kwargs):
        return self

    def string(self, *args, fallback=None, **kwargs) -> str:
        """Return fallback string for corrupted value."""
        return fallback if fallback is not None else f"(corrupted@{hex(self._addr)})"

    # -- Terminal ops: raise CorruptedError / AttributeError --

    def __getattr__(self, key):
        if key.startswith("__") and key.endswith("__"):
            raise AttributeError(key)
        raise CorruptedError(self._addr, self._error)

    def __getitem__(self, key):
        raise CorruptedError(self._addr, self._error)


# Type alias for all wrapper class __init__ parameters
ValueInput = Union[str, int, gdb.Value, Value]
