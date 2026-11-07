"""Runtime access to the field metadata behind the generated wrappers.

The wrappers carry the values; what the values *mean* - which fields matter
first, what an enum's numbers are called, how a widget summarises itself - lives
in the generator's widget_specs.json. The dashboard already ships that file to
the browser, so reading the same file here keeps the terminal output and the
HTML inspector telling one story instead of two that drift apart.

A pip-installed package carries its own copy next to lvglgdb/__init__.py
(setup.py puts it there); a source checkout reads the generator's file directly.
Everything degrades to the raw value when neither is found.
"""

import json
from pathlib import Path

_PKG_DIR = Path(__file__).resolve().parents[2]


def specs_path() -> Path:
    """Where widget_specs.json is: the packaged copy, else the checkout's."""
    packaged = _PKG_DIR / "widget_specs.json"
    if packaged.exists():
        return packaged
    return _PKG_DIR.parent / "scripts" / "generators" / "widget_specs.json"


_specs = None


def _load() -> dict:
    global _specs
    if _specs is None:
        try:
            _specs = json.loads(specs_path().read_text(encoding="utf-8"))
        except (OSError, ValueError):
            _specs = {}
    return _specs


def widget_spec(class_name: str) -> dict:
    """The spec for a class name, with or without the 'lv_' prefix."""
    specs = _load()
    return specs.get(class_name) or specs.get("lv_" + class_name) or {}


def field_types(class_name: str) -> dict:
    """Map of field name -> generator field type ('int', 'bool', 'enum:4', ...)."""
    return widget_spec(class_name).get("_auto", {}).get("fields", {})


def order_fields(class_name: str, keys) -> list:
    """Order widget_data keys: the spec's primary fields first, rest after."""
    primary = widget_spec(class_name).get("primary", [])
    keys = list(keys)
    head = [k for k in primary if k in keys]
    return head + [k for k in keys if k not in head]


def format_value(class_name: str, field: str, value) -> str:
    """Render one widget_data value: enums by name, points and areas inline."""
    if value is None:
        return "-"

    names = widget_spec(class_name).get("enums", {}).get(field)
    if names is not None and isinstance(value, int) and not isinstance(value, bool):
        if 0 <= value < len(names):
            return f"{names[value]} ({value})"
        return str(value)

    if isinstance(value, dict):
        if {"x1", "y1", "x2", "y2"} <= value.keys():
            return (
                f"({value['x1']},{value['y1']})-({value['x2']},{value['y2']})"
            )
        if {"x", "y"} <= value.keys():
            return f"({value['x']},{value['y']})"

    if field_types(class_name).get(field) == "bool":
        return str(bool(value))

    if isinstance(value, str):
        return f'"{escape(value)}"'

    return str(value)


def escape(text: str) -> str:
    """Keep a value on one line: a label's text may well be several."""
    return text.replace("\\", "\\\\").replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t")


def summary(class_name: str, widget_data: dict, limit: int = 32) -> str:
    """One-line gist of a widget, from the spec's summary template."""
    tpl = widget_spec(class_name).get("summary_tpl")
    if not tpl or not widget_data:
        return ""

    import re

    def _sub(m):
        value = widget_data.get(m.group(1))
        if value is None:
            return ""
        text = escape(str(value))
        return text[:limit] + "…" if len(text) > limit else text

    return re.sub(r"\{(\w+)\}", _sub, tpl).strip()
