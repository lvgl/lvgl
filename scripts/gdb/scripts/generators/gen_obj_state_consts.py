#!/usr/bin/env python3
"""Generate object state constant tables from LVGL headers."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_bitmask_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "core" / "lv_obj_state_consts.py"

SKIP = {"LV_STATE_ANY"}


def main():
    obj_states = parse_bitmask_enum(
        LVGL_SRC / "core" / "lv_obj_style.h",
        "lv_state_t",
        "LV_STATE_",
        skip=SKIP,
    )
    # DEFAULT = 0 is not a bitmask entry, add manually
    obj_states[0] = "DEFAULT"

    src = generate_dict_module(
        "object state constants from LVGL headers",
        {"OBJ_STATE_NAMES": obj_states},
    )
    src += (
        "\ndef decode_obj_states(raw: int) -> list[str]:\n"
        '    """Decode a bitmask of lv_state_t into a list of state names."""\n'
        "    if raw == 0:\n"
        '        return ["DEFAULT"]\n'
        "    return [name for bit, name in OBJ_STATE_NAMES.items() if bit and raw & bit]\n"
    )
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT.name} ({len(obj_states)} obj states)")


if __name__ == "__main__":
    main()
