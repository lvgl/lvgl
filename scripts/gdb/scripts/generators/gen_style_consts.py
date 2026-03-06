#!/usr/bin/env python3
"""Generate style constant tables from LVGL headers."""

import re
from pathlib import Path

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
GDB_ROOT = Path(__file__).parent.parent.parent
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "misc" / "lv_style_consts.py"

STYLE_H = LVGL_SRC / "misc" / "lv_style.h"
OBJ_STYLE_H = LVGL_SRC / "core" / "lv_obj_style.h"
STYLE_GEN_H = LVGL_SRC / "misc" / "lv_style_gen.h"

SKIP_PROPS = {
    "LV_STYLE_PROP_INV",
    "LV_STYLE_PROP_ANY",
    "LV_STYLE_PROP_CONST",
    "LV_STYLE_LAST_BUILT_IN_PROP",
    "LV_STYLE_NUM_BUILT_IN_PROPS",
}


def parse_style_props(path: Path) -> dict[int, str]:
    """Parse _lv_style_id_t enum from lv_style.h."""
    text = path.read_text()
    m = re.search(r"enum\s+_lv_style_id_t\s*\{(.*?)\}", text, re.DOTALL)
    if not m:
        raise RuntimeError("Cannot find _lv_style_id_t enum")

    props = {}
    current_val = 0
    for line in m.group(1).splitlines():
        line = line.strip().rstrip(",")
        if not line or line.startswith(("/*", "//", "*")):
            continue
        match = re.match(r"(LV_STYLE_\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line)
        if match:
            name, current_val = match.group(1), int(match.group(2), 0)
        else:
            match = re.match(r"(LV_STYLE_\w+)", line)
            if not match:
                continue
            name = match.group(1)
        if name in SKIP_PROPS:
            current_val += 1
            continue
        props[current_val] = name.removeprefix("LV_STYLE_")
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
        val = int(match.group(2), 0)
        parts[val >> 16] = match.group(1).removeprefix("LV_PART_")
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
        if name in ("LV_STATE_DEFAULT", "LV_STATE_ANY"):
            continue
        val = _parse_int_expr(match.group(2))
        if val is not None:
            states[val] = name.removeprefix("LV_STATE_")
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


def generate(props, parts, states, color_ids, pointer_ids) -> str:
    """Generate Python source for the style constants module."""
    lines = [
        '"""',
        "Auto-generated style constants from LVGL headers.",
        "",
        "Do not edit manually. Regenerate from the GDB script root with:",
        "    python3 scripts/generate_all.py",
        '"""',
        "",
    ]
    lines.append("STYLE_PROP_NAMES = {")
    for k in sorted(props):
        lines.append(f'    {k}: "{props[k]}",')
    lines.append("}")
    lines.append("")

    lines.append("PART_NAMES = {")
    for k in sorted(parts):
        lines.append(f'    0x{k:02X}: "{parts[k]}",')
    lines.append("}")
    lines.append("")

    lines.append("STATE_FLAGS = {")
    for k in sorted(states):
        lines.append(f'    0x{k:04X}: "{states[k]}",')
    lines.append("}")
    lines.append("")

    if color_ids:
        lines.append("COLOR_PROPS = {")
        for v in sorted(color_ids):
            lines.append(f"    {v},  # {props.get(v, '?')}")
        lines.append("}")
    else:
        lines.append("COLOR_PROPS = set()")
    lines.append("")

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
        f"Generated {OUTPUT.name} ({len(props)} props, {len(parts)} parts, "
        f"{len(states)} states, {len(color_ids)} color, {len(pointer_ids)} pointer)"
    )


if __name__ == "__main__":
    main()
