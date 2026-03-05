#!/usr/bin/env python3
"""
Generate indev constant tables from LVGL header files.

Parses lv_indev.h for indev type enum.

Usage:
    python3 scripts/gen_indev_consts.py
"""

import re
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GDB_ROOT = SCRIPT_DIR.parent
LVGL_SRC = GDB_ROOT.parent.parent / "src"
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "core" / "lv_indev_consts.py"

INDEV_H = LVGL_SRC / "indev" / "lv_indev.h"


def parse_enum(path: Path, enum_type: str, prefix: str) -> dict[int, str]:
    """Parse a C enum from a header file."""
    text = path.read_text()

    pattern = rf"\}}\s*{re.escape(enum_type)}\s*;"
    m = re.search(rf"typedef\s+enum\s*\{{(.*?){pattern}", text, re.DOTALL)
    if not m:
        raise RuntimeError(f"Cannot find {enum_type} enum in {path}")

    entries = {}
    current_val = 0
    for line in m.group(1).splitlines():
        line = line.strip().rstrip(",")
        if (
            not line
            or line.startswith("/*")
            or line.startswith("//")
            or line.startswith("*")
            or line.startswith("#")
        ):
            continue

        match = re.match(rf"({re.escape(prefix)}\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line)
        if match:
            name = match.group(1)
            current_val = int(match.group(2), 0)
        else:
            match = re.match(rf"({re.escape(prefix)}\w+)", line)
            if not match:
                continue
            name = match.group(1)

        short = name.removeprefix(prefix)
        entries[current_val] = short
        current_val += 1

    return entries


def generate(indev_types: dict[int, str]) -> str:
    """Generate Python source for the indev constants module."""
    lines = [
        '"""',
        "Auto-generated indev constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate with:",
        "    python3 scripts/gen_indev_consts.py",
        '"""',
        "",
        "INDEV_TYPE_NAMES = {",
    ]
    for k in sorted(indev_types):
        lines.append(f'    {k}: "{indev_types[k]}",')
    lines.append("}")
    lines.append("")

    return "\n".join(lines)


def main():
    indev_types = parse_enum(INDEV_H, "lv_indev_type_t", "LV_INDEV_TYPE_")
    src = generate(indev_types)
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT} ({len(indev_types)} indev types)")


if __name__ == "__main__":
    main()
