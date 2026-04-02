"""
Auto-generated object state constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

OBJ_STATE_NAMES = {
    0: "DEFAULT",
    1: "ALT",
    4: "CHECKED",
    8: "FOCUSED",
    16: "FOCUS_KEY",
    32: "EDITED",
    64: "HOVERED",
    128: "PRESSED",
    256: "SCROLLED",
    512: "DISABLED",
    4096: "USER_1",
    8192: "USER_2",
    16384: "USER_3",
    32768: "USER_4",
}

def decode_obj_states(raw: int) -> list[str]:
    """Decode a bitmask of lv_state_t into a list of state names."""
    if raw == 0:
        return ["DEFAULT"]
    return [name for bit, name in OBJ_STATE_NAMES.items() if bit and raw & bit]
