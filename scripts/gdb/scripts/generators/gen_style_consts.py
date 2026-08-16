#!/usr/bin/env python3
"""Generate style constant tables from LVGL headers."""

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_enum

LVGL_INC = Path(__file__).parent.parent.parent.parent.parent / "include" / "lvgl"
LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
GDB_ROOT = Path(__file__).parent.parent.parent
OUTPUT = GDB_ROOT / "lvglgdb" / "lvgl" / "misc" / "lv_style_consts.py"

STYLE_H = LVGL_INC / "core" / "lv_style.h"
OBJ_STYLE_H = LVGL_INC / "core" / "lv_obj_style.h"
STYLE_GEN_H = LVGL_INC / "core" / "lv_style_gen.h"
AREA_H = LVGL_INC / "core" / "lv_area.h"

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


def _props_by_value_type(
    style_gen_h: Path, prop_map: dict[int, str], type_pattern: str
) -> set[int]:
    """Identify properties whose lv_style_gen.h setter takes the given value type."""
    text = style_gen_h.read_text()
    name_to_id = {v: k for k, v in prop_map.items()}
    ids = set()
    for match in re.finditer(
        rf"void\s+lv_style_set_(\w+)\s*\([^,]+,\s*{type_pattern}\s+value\s*\)", text
    ):
        prop_name = match.group(1).upper()
        if prop_name in name_to_id:
            ids.add(name_to_id[prop_name])
    return ids


def parse_pointer_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify pointer properties from lv_style_gen.h setter signatures.

    Matches any pointer type, so `const int32_t *` (the grid templates) is
    classified as a pointer rather than read as a number.
    """
    return _props_by_value_type(style_gen_h, prop_map, r"(?:const\s+)?\w+\s*\*")


def parse_src_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify `const void *` properties: the image sources.

    Their value may be an `lv_image_dsc_t *`, a file path or an `LV_SYMBOL_*`
    string, so the formatter is allowed to try reading them as C strings.
    """
    return _props_by_value_type(style_gen_h, prop_map, r"const\s+void\s*\*")


def parse_coord_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify `int32_t` properties: the ones that may hold LV_PCT()/LV_SIZE_CONTENT."""
    return _props_by_value_type(style_gen_h, prop_map, r"int32_t")


def parse_bool_props(style_gen_h: Path, prop_map: dict[int, str]) -> set[int]:
    """Identify `bool` properties."""
    return _props_by_value_type(style_gen_h, prop_map, r"bool")


def parse_enum_props(
    style_gen_h: Path, prop_map: dict[int, str]
) -> dict[int, dict[int, str]]:
    """Map each enum-valued property to its {value: name} table.

    The setter signature names the enum type; the type's own header is found by
    searching the include tree for its typedef.
    """
    text = style_gen_h.read_text()
    name_to_id = {v: k for k, v in prop_map.items()}
    headers = list(LVGL_INC.rglob("*.h"))
    enum_tables: dict[str, dict[int, str]] = {}
    result = {}

    for match in re.finditer(
        r"void\s+lv_style_set_(\w+)\s*\([^,]+,\s*(lv_\w+_t)\s+value\s*\)", text
    ):
        prop_name, enum_type = match.group(1).upper(), match.group(2)
        if prop_name not in name_to_id or enum_type in ("lv_color_t", "lv_opa_t"):
            continue
        if enum_type not in enum_tables:
            enum_tables[enum_type] = _parse_named_enum(headers, enum_type)
        if enum_tables[enum_type]:
            result[name_to_id[prop_name]] = enum_tables[enum_type]
    return result


def _parse_named_enum(headers: list[Path], enum_type: str) -> dict[int, str]:
    """Find and parse an enum typedef by name, {} if it cannot be found."""
    needle = f"}} {enum_type};"
    prefix = enum_type.removesuffix("_t").upper() + "_"
    for header in headers:
        if needle not in header.read_text():
            continue
        try:
            return parse_enum(header, enum_type, prefix)
        except RuntimeError:
            return {}
    return {}


def parse_coord_shift(area_h: Path) -> int:
    """Read LV_COORD_TYPE_SHIFT, which defines the LV_PCT()/content encoding."""
    m = re.search(r"#define\s+LV_COORD_TYPE_SHIFT\s+\((\d+)U?\)", area_h.read_text())
    if not m:
        raise RuntimeError("Cannot find LV_COORD_TYPE_SHIFT")
    return int(m.group(1))


def generate(props, parts, states, color_ids, pointer_ids, src_ids, coord_ids,
             bool_ids, enum_values, coord_shift) -> str:
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

    for name, ids in (
        ("POINTER_PROPS", pointer_ids),
        ("SRC_PROPS", src_ids),
        ("COORD_PROPS", coord_ids),
        ("BOOL_PROPS", bool_ids),
    ):
        if ids:
            lines.append(f"{name} = {{")
            for v in sorted(ids):
                lines.append(f"    {v},  # {props.get(v, '?')}")
            lines.append("}")
        else:
            lines.append(f"{name} = set()")
        lines.append("")

    lines.append("ENUM_PROP_VALUES = {")
    for prop_id in sorted(enum_values):
        lines.append(f"    {prop_id}: {{  # {props.get(prop_id, '?')}")
        for val in sorted(enum_values[prop_id]):
            lines.append(f'        {val}: "{enum_values[prop_id][val]}",')
        lines.append("    },")
    lines.append("}")
    lines.append("")

    lines.append("# From LV_COORD_TYPE_SHIFT in lv_area.h")
    lines.append(f"COORD_TYPE_SHIFT = {coord_shift}")
    lines.append("")

    return "\n".join(lines)


def main():
    props = parse_style_props(STYLE_H)
    parts = parse_parts(OBJ_STYLE_H)
    states = parse_states(OBJ_STYLE_H)
    color_ids = parse_color_props(STYLE_GEN_H, props)
    pointer_ids = parse_pointer_props(STYLE_GEN_H, props)
    src_ids = parse_src_props(STYLE_GEN_H, props)
    coord_ids = parse_coord_props(STYLE_GEN_H, props)
    bool_ids = parse_bool_props(STYLE_GEN_H, props)
    enum_values = parse_enum_props(STYLE_GEN_H, props)
    coord_shift = parse_coord_shift(AREA_H)

    src = generate(props, parts, states, color_ids, pointer_ids, src_ids,
                   coord_ids, bool_ids, enum_values, coord_shift)
    OUTPUT.write_text(src)
    print(
        f"Generated {OUTPUT.name} ({len(props)} props, {len(parts)} parts, "
        f"{len(states)} states, {len(color_ids)} color, {len(pointer_ids)} pointer, "
        f"{len(coord_ids)} coord, {len(bool_ids)} bool, {len(enum_values)} enum)"
    )


if __name__ == "__main__":
    main()
