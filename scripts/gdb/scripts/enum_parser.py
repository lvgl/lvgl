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
    # #defines of the header, plus the members already seen: an enum member is
    # commonly defined in terms of earlier ones.
    known = _defines(text)
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

        match = re.match(rf"({re.escape(prefix)}\w+)\s*=\s*(.+)$", line)
        if match:
            name = match.group(1)
            # A comment can start on the same line as the value, and a Doxygen
            # one often runs on to the next; only the expression before it is
            # the value. LV_EVENT_PREPROCESS = 0x8000 is written that way. A
            # comment that closes again can also sit between two operands, so
            # drop those first rather than truncating the expression at one.
            expression = re.sub(r"/\*.*?\*/", " ", match.group(2))
            expression = re.split(r"//|/\*", expression, maxsplit=1)[0]
            value = eval_c_expr(expression.rstrip().rstrip(","), known)
            if value is None:
                # Falling back to the running counter would give this member a
                # plausible wrong number and shift every implicit member after
                # it, which is impossible to notice in the generated table.
                raise ValueError(
                    f"{name} is assigned `{expression.strip()}`, which this "
                    f"parser cannot evaluate. Teach eval_c_expr about it rather "
                    f"than letting the value be guessed."
                )
            current_val = value
        else:
            match = re.match(rf"({re.escape(prefix)}\w+)", line)
            if not match:
                continue
            name = match.group(1)

        # A skipped member is still a member: a later one may be written in
        # terms of it, so it has to be resolvable even though it is left out
        # of the table.
        known[name] = current_val

        if name in skip:
            current_val += 1
            continue

        short = name.removeprefix(prefix)
        entries[current_val] = short
        current_val += 1

    return entries


_DEFINE = re.compile(r"^#define\s+(_?LV_[A-Z0-9_]+)\s+(.+)$", re.M)


def _defines(text: str) -> dict:
    """The #define constants of a header, so enum values can reference them."""
    return {name: value.split("/*")[0].strip()
            for name, value in _DEFINE.findall(text)}


def eval_c_expr(expr: str, known: dict) -> "int | None":
    """Evaluate a C constant expression, or None if it cannot be resolved.

    Enum members are regularly defined in terms of others -
    `LV_FLEX_FLOW_ROW_WRAP = LV_FLEX_FLOW_ROW | LV_FLEX_WRAP` - and a parser
    that only accepts literals silently numbers them sequentially instead,
    which turns a bitmask into consecutive integers.
    """
    expr = expr.strip().rstrip(",")
    if not expr:
        return None
    for _ in range(8):
        replaced = re.sub(
            r"_?LV_[A-Z0-9_]+",
            lambda m: f"({known[m.group(0)]})" if m.group(0) in known else m.group(0),
            expr,
        )
        if replaced == expr:
            break
        expr = replaced
    # Integer suffixes, then blank out hex literals so their letters are not
    # mistaken for an unresolved identifier.
    expr = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", expr)
    if re.search(r"[A-Za-z_]", re.sub(r"0[xX][0-9a-fA-F]+", "0", expr)):
        return None
    if not re.fullmatch(r"[\d\sxXa-fA-F|&^~<>+\-*/()]+", expr):
        return None
    try:
        value = int(eval(expr, {"__builtins__": {}}, {}))
    except Exception:
        return None
    # Python's ints are unbounded, so ~0x0F is -16 where C's 32-bit unsigned
    # gives 0xFFFFFFF0. Only complement can produce that difference here: the
    # other operators agree for the values LVGL's headers hold.
    if "~" in expr and value < 0:
        value &= 0xFFFFFFFF
    return value


def parse_bitmask_enum(path: Path, enum_type: str, prefix: str,
                       skip: set[str] | None = None) -> dict[int, str]:
    """Parse a C typedef enum with bitmask values (1u << N) from a header.

    Only entries with explicit ``(1u << N)`` assignments are collected.
    Entries whose value references other enum members (aliases / combos)
    are silently skipped.

    Args:
        path: Path to the C header file.
        enum_type: The typedef name (e.g. "lv_obj_flag_t").
        prefix: Enum member prefix to strip (e.g. "LV_OBJ_FLAG_").
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

        # Match: NAME = (1u << N) or NAME = 1 << N (parens optional), and only
        # that. Without anchoring the end, `NAME = (1 << 5) | (1 << 6)` would
        # match its first shift and be recorded as a single-bit flag, which is
        # worse than leaving a combination out of a bit table.
        match = re.match(
            rf"({re.escape(prefix)}\w+)\s*=\s*\(?1u?\s*<<\s*(\d+)\)?"
            r"\s*,?\s*(?:/[/*].*)?$",
            line,
        )
        if not match:
            continue

        name = match.group(1)
        if name in skip:
            continue

        bit = int(match.group(2))
        short = name.removeprefix(prefix)
        entries[1 << bit] = short

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
