#!/usr/bin/env python3
"""Generate subject type constant table from LVGL headers."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "core" / "lv_observer_consts.py"


def main():
    subject_types = parse_enum(
        LVGL_SRC / "core" / "lv_observer.h",
        "lv_subject_type_t",
        "LV_SUBJECT_TYPE_",
    )
    src = generate_dict_module(
        "observer constants from LVGL headers",
        {"SUBJECT_TYPE_NAMES": subject_types},
    )
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT.name} ({len(subject_types)} subject types)")


if __name__ == "__main__":
    main()
