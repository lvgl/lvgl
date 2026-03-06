#!/usr/bin/env python3
"""
Generate subject type constant table from LVGL header files.

Parses lv_observer.h for subject type enum.

Usage:
    python3 scripts/gen_subject_consts.py
"""

import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GDB_ROOT = SCRIPT_DIR.parent
LVGL_SRC = GDB_ROOT.parent.parent / "src"
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "core" / "lv_observer_consts.py"

OBSERVER_H = LVGL_SRC / "core" / "lv_observer.h"

sys.path.insert(0, str(SCRIPT_DIR))
from gen_indev_consts import parse_enum


def generate(subject_types: dict[int, str]) -> str:
    """Generate Python source for the observer constants module."""
    lines = [
        '"""',
        "Auto-generated observer constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate with:",
        "    python3 scripts/gen_subject_consts.py",
        '"""',
        "",
        "SUBJECT_TYPE_NAMES = {",
    ]
    for k in sorted(subject_types):
        lines.append(f'    {k}: "{subject_types[k]}",')
    lines.append("}")
    lines.append("")

    return "\n".join(lines)


def main():
    subject_types = parse_enum(OBSERVER_H, "lv_subject_type_t", "LV_SUBJECT_TYPE_")
    src = generate(subject_types)
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT} ({len(subject_types)} subject types)")


if __name__ == "__main__":
    main()
