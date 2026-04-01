"""
Auto-generated object flag constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

OBJ_FLAG_NAMES = {
    1: "HIDDEN",
    2: "CLICKABLE",
    4: "CLICK_FOCUSABLE",
    8: "CHECKABLE",
    16: "SCROLLABLE",
    32: "SCROLL_ELASTIC",
    64: "SCROLL_MOMENTUM",
    128: "SCROLL_ONE",
    256: "SCROLL_CHAIN_HOR",
    512: "SCROLL_CHAIN_VER",
    1024: "SCROLL_ON_FOCUS",
    2048: "SCROLL_WITH_ARROW",
    4096: "SNAPPABLE",
    8192: "PRESS_LOCK",
    16384: "EVENT_BUBBLE",
    32768: "GESTURE_BUBBLE",
    65536: "ADV_HITTEST",
    131072: "IGNORE_LAYOUT",
    262144: "FLOATING",
    524288: "SEND_DRAW_TASK_EVENTS",
    1048576: "OVERFLOW_VISIBLE",
    2097152: "EVENT_TRICKLE",
    4194304: "STATE_TRICKLE",
    8388608: "LAYOUT_1",
    16777216: "LAYOUT_2",
    33554432: "WIDGET_1",
    67108864: "WIDGET_2",
    134217728: "USER_1",
    268435456: "USER_2",
    536870912: "USER_3",
    1073741824: "USER_4",
}

def decode_obj_flags(raw: int) -> list[str]:
    """Decode a bitmask of lv_obj_flag_t into a list of flag names."""
    return [name for bit, name in OBJ_FLAG_NAMES.items() if raw & bit]
