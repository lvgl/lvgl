#!/usr/bin/env python3
"""
Generate event constant tables from LVGL header files.

Parses lv_event.h for the lv_event_code_t enum.

Usage:
    python3 scripts/gen_event_consts.py
"""

import re
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GDB_ROOT = SCRIPT_DIR.parent
LVGL_SRC = GDB_ROOT.parent.parent / "src"
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "misc" / "lv_event_consts.py"

EVENT_H = LVGL_SRC / "misc" / "lv_event.h"


def parse_event_codes(path: Path) -> dict[int, str]:
    """Parse lv_event_code_t enum from lv_event.h."""
    text = path.read_text()

    m = re.search(r"typedef\s+enum\s*\{(.*?)\}\s*lv_event_code_t", text, re.DOTALL)
    if not m:
        raise RuntimeError("Cannot find lv_event_code_t enum")

    codes = {}
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

        # Match: LV_EVENT_XXX = value
        match = re.match(r"(LV_EVENT_\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line)
        if match:
            name = match.group(1)
            current_val = int(match.group(2), 0)
        else:
            match = re.match(r"(LV_EVENT_\w+)", line)
            if not match:
                continue
            name = match.group(1)

        # Skip meta entries
        if name in ("LV_EVENT_LAST", "LV_EVENT_PREPROCESS", "LV_EVENT_MARKED_DELETING"):
            current_val += 1
            continue

        short = name.removeprefix("LV_EVENT_")
        codes[current_val] = short
        current_val += 1

    return codes


def generate(codes: dict[int, str]) -> str:
    """Generate Python source for the event constants module."""
    lines = [
        '"""',
        "Auto-generated event constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate with:",
        "    python3 scripts/gen_event_consts.py",
        '"""',
        "",
    ]

    lines.append("EVENT_CODE_NAMES = {")
    for k in sorted(codes):
        lines.append(f'    {k}: "{codes[k]}",')
    lines.append("}")
    lines.append("")

    return "\n".join(lines)


def main():
    codes = parse_event_codes(EVENT_H)
    src = generate(codes)
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT} ({len(codes)} event codes)")


if __name__ == "__main__":
    main()
