#!/usr/bin/env python3
"""Generate indev constant tables from LVGL headers."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "core" / "lv_indev_consts.py"


def main():
    indev_types = parse_enum(
        LVGL_SRC / "indev" / "lv_indev.h",
        "lv_indev_type_t",
        "LV_INDEV_TYPE_",
    )
    src = generate_dict_module(
        "indev constants from LVGL headers",
        {"INDEV_TYPE_NAMES": indev_types},
    )
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT.name} ({len(indev_types)} indev types)")


if __name__ == "__main__":
    main()
