#!/usr/bin/env python3
"""
Shared utilities for LVGL constant generators.

Provides:
  - parse_enum(): Parse a C typedef enum from a header file.
  - generate_dict_module(): Generate a Python module with dict constants.
"""

import re
from pathlib import Path


def parse_enum(path: Path, enum_type: str, prefix: str,
               skip: set[str] | None = None) -> dict[int, str]:
    """Parse a C typedef enum from a header file.

    Args:
        path: Path to the C header file.
        enum_type: The typedef name (e.g. "lv_indev_type_t").
        prefix: Enum member prefix to strip (e.g. "LV_INDEV_TYPE_").
        skip: Optional set of full enum member names to skip.

    Returns:
        Dict mapping int value -> short name string.
    """
    text = path.read_text()
    skip = skip or set()

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

        match = re.match(
            rf"({re.escape(prefix)}\w+)\s*=\s*(0x[\da-fA-F]+|\d+)", line
        )
        if match:
            name = match.group(1)
            current_val = int(match.group(2), 0)
        else:
            match = re.match(rf"({re.escape(prefix)}\w+)", line)
            if not match:
                continue
            name = match.group(1)

        if name in skip:
            current_val += 1
            continue

        short = name.removeprefix(prefix)
        entries[current_val] = short
        current_val += 1

    return entries


def generate_dict_module(
    description: str,
    dicts: dict[str, dict],
) -> str:
    """Generate a Python module containing one or more dict constants.

    Args:
        description: Short description for the module docstring.
        dicts: Mapping of variable_name -> dict to emit.

    Returns:
        Python source code string.
    """
    lines = [
        '"""',
        f"Auto-generated {description}.",
        "",
        "Do not edit manually. Regenerate from the GDB script root with:",
        "    python3 scripts/generate_all.py",
        '"""',
        "",
    ]

    for var_name, data in dicts.items():
        lines.append(f"{var_name} = {{")
        for k in sorted(data):
            v = data[k]
            if isinstance(k, int):
                lines.append(f'    {k}: "{v}",')
            else:
                lines.append(f'    "{k}": "{v}",')
        lines.append("}")
        lines.append("")

    return "\n".join(lines)
