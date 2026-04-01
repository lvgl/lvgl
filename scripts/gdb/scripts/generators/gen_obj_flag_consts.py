#!/usr/bin/env python3
"""Generate object flag constant tables from LVGL headers."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_bitmask_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "core" / "lv_obj_flag_consts.py"


def main():
    obj_flags = parse_bitmask_enum(
        LVGL_SRC / "core" / "lv_obj.h",
        "lv_obj_flag_t",
        "LV_OBJ_FLAG_",
    )

    # Add decode helper after the dict
    src = generate_dict_module(
        "object flag constants from LVGL headers",
        {"OBJ_FLAG_NAMES": obj_flags},
    )
    src += (
        "\ndef decode_obj_flags(raw: int) -> list[str]:\n"
        '    """Decode a bitmask of lv_obj_flag_t into a list of flag names."""\n'
        "    return [name for bit, name in OBJ_FLAG_NAMES.items() if raw & bit]\n"
    )
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT.name} ({len(obj_flags)} obj flags)")


if __name__ == "__main__":
    main()
