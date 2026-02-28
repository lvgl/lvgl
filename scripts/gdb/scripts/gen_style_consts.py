#!/usr/bin/env python3
"""
Generate style constant tables from LVGL header files.

Parses lv_style.h, lv_obj_style.h, and lv_style_gen.h to produce
lv_style_consts.py used by the lvglgdb GDB plugin.

Usage:
    python3 scripts/gen_style_consts.py
"""

import re
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GDB_ROOT = SCRIPT_DIR.parent
LVGL_SRC = GDB_ROOT.parent.parent / "src"
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "misc" / "lv_style_consts.py"

STYLE_H = LVGL_SRC / "misc" / "lv_style.h"
OBJ_STYLE_H = LVGL_SRC / "core" / "lv_obj_style.h"
STYLE_GEN_H = LVGL_SRC / "misc" / "lv_style_gen.h"


def parse_style_props(path: Path) -> dict[int, str]:
    """Parse _lv_style_id_t enum from lv_style.h."""
    text = path.read_text()

    # Extract enum block
    m = re.search(r"enum\s+_lv_style_id_t\s*\{(.*?)\}", text, re.DOTALL)
    if not m:
        raise RuntimeError("Cannot find _lv_style_id_t enum")

    props = {}
    current_val = 0
    for line in m.group(1).splitlines():
        line = line.strip().rstrip(",")
        if (
            not line
            or line.startswith("/*")
            or line.startswith("//")
            or line.startswith("*")
        ):
            continue

        # Match: LV_STYLE_XXX = value  or  LV_STYLE_XXX (auto-increment)
        match = re.match(r"(LV_STYLE_\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line)
        if match:
            name = match.group(1)
            val_str = match.group(2)
            current_val = int(val_str, 0)
        else:
            match = re.match(r"(LV_STYLE_\w+)", line)
            if not match:
                continue
            name = match.group(1)

        # Skip meta entries
        if name in (
            "LV_STYLE_PROP_INV",
            "LV_STYLE_PROP_ANY",
            "LV_STYLE_PROP_CONST",
            "LV_STYLE_LAST_BUILT_IN_PROP",
            "LV_STYLE_NUM_BUILT_IN_PROPS",
        ):
            current_val += 1
            continue

        short = name.removeprefix("LV_STYLE_")
        props[current_val] = short
        current_val += 1

    return props


def parse_parts(path: Path) -> dict[int, str]:
    """Parse lv_part_t enum from lv_obj_style.h."""
    text = path.read_text()
    m = re.search(r"typedef\s+enum\s*\{(.*?)\}\s*lv_part_t", text, re.DOTALL)
    if not m:
        raise RuntimeError("Cannot find lv_part_t enum")

    parts = {}
    for line in m.group(1).splitlines():
        match = re.match(r"\s*(LV_PART_\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line)
        if not match:
            continue
        name = match.group(1)
        val = int(match.group(2), 0)
        short = name.removeprefix("LV_PART_")
        # Selector uses bits [23:16], shift down to get the key
        parts[val >> 16] = short

    return parts


def _parse_int_expr(expr: str) -> int | None:
    """Parse a C integer expression: decimal, hex, or '1 << N'."""
    expr = expr.strip()
    m = re.match(r"^(0x[\da-fA-F]+|\d+)$", expr)
    if m:
        return int(m.group(1), 0)
    m = re.match(r"^(\d+)\s*<<\s*(\d+)$", expr)
    if m:
        return int(m.group(1)) << int(m.group(2))
    return None


def parse_states(path: Path) -> dict[int, str]:
    """Parse lv_state_t enum from lv_obj_style.h."""
    text = path.read_text()
    m = re.search(r"typedef\s+enum\s*\{(.*?)\}\s*lv_state_t", text, re.DOTALL)
    if not m:
        raise RuntimeError("Cannot find lv_state_t enum")

    states = {}
    for line in m.group(1).splitlines():
        match = re.match(r"\s*(LV_STATE_\w+)\s*=\s*(.+?)(?:,|/)", line)
        if not match:
            continue
        name = match.group(1)
        expr = match.group(2).strip()

        # Skip DEFAULT (0) and ANY (0xFFFF)
        if name in ("LV_STATE_DEFAULT", "LV_STATE_ANY"):
            continue

        val = _parse_int_expr(expr)
        if val is None:
            continue
        short = name.removeprefix("LV_STATE_")
        states[val] = short

    return states


def parse_color_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify color properties from lv_style_gen.h setter signatures."""
    text = style_gen_h.read_text()
    name_to_id = {v: k for k, v in prop_map.items()}

    color_ids = set()
    for match in re.finditer(
        r"void\s+lv_style_set_(\w+)\s*\([^,]+,\s*lv_color_t", text
    ):
        prop_name = match.group(1).upper()
        if prop_name in name_to_id:
            color_ids.add(name_to_id[prop_name])

    return color_ids


def parse_pointer_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify pointer properties from lv_style_gen.h setter signatures."""
    text = style_gen_h.read_text()
    name_to_id = {v: k for k, v in prop_map.items()}

    ptr_ids = set()
    for match in re.finditer(
        r"void\s+lv_style_set_(\w+)\s*\([^,]+,\s*(?:const\s+)?(?:void|lv_\w+)\s*\*",
        text,
    ):
        prop_name = match.group(1).upper()
        if prop_name in name_to_id:
            ptr_ids.add(name_to_id[prop_name])

    return ptr_ids


def generate(
    props: dict[int, str],
    parts: dict[int, str],
    states: dict[int, str],
    color_ids: set[int],
    pointer_ids: set[int],
) -> str:
    """Generate Python source for the constants module."""
    lines = [
        '"""',
        "Auto-generated style constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate with:",
        "    python3 scripts/gen_style_consts.py",
        '"""',
        "",
    ]

    # STYLE_PROP_NAMES
    lines.append("STYLE_PROP_NAMES = {")
    for k in sorted(props):
        lines.append(f'    {k}: "{props[k]}",')
    lines.append("}")
    lines.append("")

    # PART_NAMES
    lines.append("PART_NAMES = {")
    for k in sorted(parts):
        lines.append(f'    0x{k:02X}: "{parts[k]}",')
    lines.append("}")
    lines.append("")

    # STATE_FLAGS
    lines.append("STATE_FLAGS = {")
    for k in sorted(states):
        lines.append(f'    0x{k:04X}: "{states[k]}",')
    lines.append("}")
    lines.append("")

    # COLOR_PROPS
    if color_ids:
        lines.append("COLOR_PROPS = {")
        for v in sorted(color_ids):
            lines.append(f"    {v},  # {props.get(v, '?')}")
        lines.append("}")
    else:
        lines.append("COLOR_PROPS = set()")
    lines.append("")

    # POINTER_PROPS
    if pointer_ids:
        lines.append("POINTER_PROPS = {")
        for v in sorted(pointer_ids):
            lines.append(f"    {v},  # {props.get(v, '?')}")
        lines.append("}")
    else:
        lines.append("POINTER_PROPS = set()")
    lines.append("")

    return "\n".join(lines)


def main():
    props = parse_style_props(STYLE_H)
    parts = parse_parts(OBJ_STYLE_H)
    states = parse_states(OBJ_STYLE_H)
    color_ids = parse_color_props(STYLE_GEN_H, props)
    pointer_ids = parse_pointer_props(STYLE_GEN_H, props)

    src = generate(props, parts, states, color_ids, pointer_ids)
    OUTPUT.write_text(src)
    print(
        f"Generated {OUTPUT} ({len(props)} props, {len(parts)} parts, "
        f"{len(states)} states, {len(color_ids)} color, {len(pointer_ids)} pointer)"
    )


if __name__ == "__main__":
    main()
