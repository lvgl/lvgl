"""Shared field-reading helpers for widget wrappers.

All helpers return a safe value (or None) without raising exceptions.
Protection is provided by Value.safe_field() and Value.string(fallback=).
"""

from lvglgdb.lvgl.data_utils import ptr_or_none  # noqa: F401


def safe_string(obj, field_name):
    """Read a char* field as string, or None."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True) or not int(val):
        return None
    return val.string(fallback=None)


def safe_color(obj, field_name):
    """Read lv_color_t as hex string."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return f"#{int(val):06x}"


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
