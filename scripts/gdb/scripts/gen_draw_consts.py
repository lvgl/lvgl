#!/usr/bin/env python3
"""
Generate draw constant tables from LVGL header and source files.

Parses lv_draw.h for task type/state enums, and scans draw unit source
files for name-to-struct-type mappings.

Usage:
    python3 scripts/gen_draw_consts.py
"""

import re
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GDB_ROOT = SCRIPT_DIR.parent
LVGL_SRC = GDB_ROOT.parent.parent / "src"
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "draw" / "lv_draw_consts.py"

DRAW_H = LVGL_SRC / "draw" / "lv_draw.h"
DRAW_DIR = LVGL_SRC / "draw"


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


def parse_draw_unit_types(draw_dir: Path) -> dict[str, str]:
    """Scan draw unit .c files for name-to-struct-type mappings.

    Looks for patterns like: unit->base_unit.name = "SW";
    Then finds the corresponding struct type from the variable declaration.
    """
    mappings = {}

    for c_file in draw_dir.rglob("*.c"):
        text = c_file.read_text()

        for m in re.finditer(r'(\w+)->base_unit\.name\s*=\s*"(\w+)"', text):
            var_name = m.group(1)
            unit_name = m.group(2)

            decl = re.search(
                rf"(lv_draw_\w+_unit_t)\s*\*\s*{re.escape(var_name)}\b", text
            )
            if decl:
                mappings[unit_name] = decl.group(1)

    return mappings


def generate(
    task_types: dict[int, str],
    task_states: dict[int, str],
    unit_types: dict[str, str],
) -> str:
    """Generate Python source for the draw constants module."""
    lines = [
        '"""',
        "Auto-generated draw constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate with:",
        "    python3 scripts/gen_draw_consts.py",
        '"""',
        "",
    ]

    lines.append("DRAW_TASK_TYPE_NAMES = {")
    for k in sorted(task_types):
        lines.append(f'    {k}: "{task_types[k]}",')
    lines.append("}")
    lines.append("")

    lines.append("DRAW_TASK_STATE_NAMES = {")
    for k in sorted(task_states):
        lines.append(f'    {k}: "{task_states[k]}",')
    lines.append("}")
    lines.append("")

    lines.append("DRAW_UNIT_TYPE_NAMES = {")
    for name in sorted(unit_types):
        lines.append(f'    "{name}": "{unit_types[name]}",')
    lines.append("}")
    lines.append("")

    return "\n".join(lines)


def main():
    task_types = parse_enum(DRAW_H, "lv_draw_task_type_t", "LV_DRAW_TASK_TYPE_")
    task_states = parse_enum(DRAW_H, "lv_draw_task_state_t", "LV_DRAW_TASK_STATE_")
    unit_types = parse_draw_unit_types(DRAW_DIR)

    src = generate(task_types, task_states, unit_types)
    OUTPUT.write_text(src)
    print(
        f"Generated {OUTPUT} ({len(task_types)} task types, "
        f"{len(task_states)} task states, {len(unit_types)} unit types)"
    )


if __name__ == "__main__":
    main()
