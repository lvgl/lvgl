"""Shared field-reading helpers for widget wrappers.

All helpers return a safe value (or None) without raising exceptions.
Protection is provided by Value.safe_field() and Value.string(fallback=).
"""

from lvglgdb.lvgl.data_utils import ptr_or_none  # noqa: F401


def safe_string(obj, field_name):
    """Read a char* field as string, corrupted marker, or None (NULL/missing)."""
    from lvglgdb.value import CorruptedValue
    val = obj.safe_field(field_name)
    if val is None:
        return None
    if not getattr(val, 'is_ok', True):
        return str(val)
    addr = int(val)
    if not addr:
        return None
    return val.string(fallback=str(CorruptedValue(addr, MemoryError("unreadable"))))


def safe_color(obj, field_name):
    """Read lv_color_t {red, green, blue} as hex string."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    r = int(val.safe_field("red", 0))
    g = int(val.safe_field("green", 0))
    b = int(val.safe_field("blue", 0))
    return f"#{r:02x}{g:02x}{b:02x}"


def safe_area(obj, field_name):
    """Read lv_area_t as dict."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x1": int(val.safe_field("x1", 0)),
        "y1": int(val.safe_field("y1", 0)),
        "x2": int(val.safe_field("x2", 0)),
        "y2": int(val.safe_field("y2", 0)),
    }


def safe_point(obj, field_name):
    """Read lv_point_t as dict."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x": int(val.safe_field("x", 0)),
        "y": int(val.safe_field("y", 0)),
    }


def safe_wrapper(obj, field_name, module_path, class_name):
    """Read a struct field using its known Value wrapper, return snapshot dict.

    Not every wrapper has a snapshot(); LVArray does not. Value forwards an
    unknown attribute to gdb.Value.__getitem__, so asking for one raises
    gdb.error rather than AttributeError, and the caller would otherwise lose
    the whole widget - and its subtree - over a single field.
    """
    import gdb

    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    import importlib
    try:
        mod = importlib.import_module(module_path)
        cls = getattr(mod, class_name)
        wrapper = cls(val)
        if getattr(type(wrapper), "snapshot", None) is None:
            # An embedded struct - lv_scale_t.needles - is not convertible to an
            # int, so the address has to come from the field itself.
            if val.type.strip_typedefs().code != gdb.TYPE_CODE_PTR:
                val = val.address
            return {"addr": hex(int(val)), "type": class_name}
        return wrapper.snapshot().as_dict()
    except (gdb.error, gdb.MemoryError, ImportError, AttributeError, TypeError):
        return None
