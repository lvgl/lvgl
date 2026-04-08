#!/usr/bin/env python3
"""Generate draw constant tables from LVGL headers and source files."""

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "draw" / "lv_draw_consts.py"

DRAW_H = LVGL_SRC / "draw" / "lv_draw.h"
DRAW_DIR = LVGL_SRC / "draw"


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


def main():
    task_types = parse_enum(DRAW_H, "lv_draw_task_type_t", "LV_DRAW_TASK_TYPE_")
    task_states = parse_enum(DRAW_H, "lv_draw_task_state_t", "LV_DRAW_TASK_STATE_")
    unit_types = parse_draw_unit_types(DRAW_DIR)

    src = generate_dict_module(
        "draw constants from LVGL headers",
        {
            "DRAW_TASK_TYPE_NAMES": task_types,
            "DRAW_TASK_STATE_NAMES": task_states,
            "DRAW_UNIT_TYPE_NAMES": unit_types,
        },
    )
    OUTPUT.write_text(src)
    print(
        f"Generated {OUTPUT.name} ({len(task_types)} task types, "
        f"{len(task_states)} task states, {len(unit_types)} unit types)"
    )


if __name__ == "__main__":
    main()
