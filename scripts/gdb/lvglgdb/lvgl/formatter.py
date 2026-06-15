"""Generic formatting helpers for snapshot data.

This module provides data-driven formatters that work with any snapshot dict.
No module-specific logic lives here — all customization is done via field specs
and format callbacks passed by callers.
"""
from typing import Callable, List, Optional, Sequence, Tuple, Union

from prettytable import PrettyTable


# ---------------------------------------------------------------------------
# Type aliases for field specs
# ---------------------------------------------------------------------------

# A field spec is a tuple describing one line of print_info output:
#   (label, key_or_fmt)
#
# - label: the left-hand label string (e.g. "var")
# - key_or_fmt: either a dict key string, or a callable(d) -> str
#
# Special forms:
#   ("_title", callable(d) -> str)   — title line (no indent)
#   ("_children", key, child_fields) — nested list of dicts
#   ("_skip_if", key, sentinel, (label, key_or_fmt))
#       — only print if d[key] != sentinel

FieldSpec = Union[
    Tuple[str, Union[str, Callable]],
    Tuple[str, str, Callable],
    Tuple[str, str, str, Tuple],
    Tuple[str, str, list],
]


def print_info(d, fields: Sequence = None, indent: int = 0) -> None:
    """Print a snapshot dict using a declarative field spec list.

    Fields resolution order:
        1. Explicit *fields* parameter (highest priority)
        2. d._display_spec["info"] (Snapshot self-describing)
        3. None → default rendering (iterate dict keys as "key = value")
    """
    if fields is None:
        spec = getattr(d, "_display_spec", None)
        if spec:
            fields = spec["info"]

    # Default rendering: iterate dict keys
    if fields is None:
        prefix = "  " * indent
        items = d.items() if hasattr(d, "items") else vars(d).items()
        for key, value in items:
            print(f"{prefix}  {key} = {value}")
        return

    prefix = "  " * indent
    for spec in fields:
        # Bare string shorthand: "key" → ("key", "key")
        if isinstance(spec, str):
            spec = (spec, spec)

        tag = spec[0]

        if tag == "_title":
            fmt_fn = spec[1]
            print(f"{prefix}{fmt_fn(d)}")

        elif tag == "_children":
            key, child_fields = spec[1], spec[2]
            children = d.get(key, [])
            for child in children:
                print_info(child, fields=child_fields, indent=indent + 1)

        elif tag == "_skip_if":
            key, sentinel, inner = spec[1], spec[2], spec[3]
            # Bare string shorthand for inner: "key" → ("key", "key")
            if isinstance(inner, str):
                inner = (inner, inner)
            if d.get(key) != sentinel:
                _print_field(prefix, inner, d)

        else:
            _print_field(prefix, spec, d)


def _print_field(prefix: str, spec: tuple, d) -> None:
    """Print a single (label, key_or_fmt) field."""
    label, key_or_fmt = spec[0], spec[1]
    if callable(key_or_fmt):
        value = key_or_fmt(d)
    else:
        value = d.get(key_or_fmt, "")
    print(f"{prefix}  {label:14s} = {value}")


# ---------------------------------------------------------------------------
# Table helpers
# ---------------------------------------------------------------------------


def print_table(
    entries: List,
    columns: List[str],
    row_fn: Callable,
    empty_msg: str,
    align: str = "l",
    numbered: bool = True,
    col_align: dict = None,
) -> None:
    """Generic helper for printing a PrettyTable from snapshot data.

    Args:
        entries: list of dict-like objects (Snapshot or plain dict).
        columns: column header names (excluding '#' if numbered=True).
        row_fn: callable(index, entry) -> list of cell values.
        empty_msg: message to print when entries is empty.
        align: default column alignment.
        numbered: if True, prepend a '#' column with row index.
        col_align: optional per-column alignment overrides, e.g. {"src": "l"}.
    """
    table = PrettyTable()
    table.field_names = (["#"] + columns) if numbered else columns
    table.align = align
    if col_align:
        for col, a in col_align.items():
            if col in table.field_names:
                table.align[col] = a

    for i, d in enumerate(entries):
        row = row_fn(i, d)
        if numbered:
            row = [i] + row
        table.add_row(row)

    if not table.rows:
        print(empty_msg)
    else:
        print(table)

def resolve_table_columns(spec: dict) -> tuple:
    """Resolve Display_Spec into (column_headers, auto_row_fn).

    Merges the ``info`` field list with ``table`` overrides to produce a flat
    list of column headers and a row-extraction function with ``(i, d)``
    signature suitable for :func:`print_table`.

    Algorithm:
        1. Walk ``spec["info"]``, skip ``_title`` / ``_children``, normalise
           bare strings, and collect ``(label, key_or_fmt, skip_info)`` triples.
        2. Walk ``spec["table"]``, build an override map (keyed by label) and
           an append list for labels not present in the base columns.
        3. Apply overrides in-place, then append new columns.
        4. Return ``(headers, auto_row_fn)``.
    """
    # -- 1. base columns from info ----------------------------------------
    base_columns = []
    base_labels = set()
    for entry in spec["info"]:
        if isinstance(entry, str):
            entry = (entry, entry)

        tag = entry[0]
        if tag in ("_title", "_children"):
            continue

        if tag == "_skip_if":
            skip_key, sentinel, inner = entry[1], entry[2], entry[3]
            if isinstance(inner, str):
                inner = (inner, inner)
            label, key_or_fmt = inner[0], inner[1]
            base_columns.append((label, key_or_fmt, (skip_key, sentinel)))
        else:
            label, key_or_fmt = entry[0], entry[1]
            base_columns.append((label, key_or_fmt, None))

        base_labels.add(base_columns[-1][0])

    # -- 2. table overrides and appends -----------------------------------
    override_map = {}
    append_list = []
    for entry in spec.get("table", []):
        if isinstance(entry, str):
            entry = (entry, entry)

        tag = entry[0]
        if tag == "_skip_if":
            skip_key, sentinel, inner = entry[1], entry[2], entry[3]
            if isinstance(inner, str):
                inner = (inner, inner)
            label, key_or_fmt = inner[0], inner[1]
            item = (label, key_or_fmt, (skip_key, sentinel))
        else:
            label, key_or_fmt = entry[0], entry[1]
            item = (label, key_or_fmt, None)

        if label in base_labels:
            override_map[label] = item
        else:
            append_list.append(item)

    # -- 3. merge ---------------------------------------------------------
    resolved = []
    for label, key_or_fmt, skip in base_columns:
        if label in override_map:
            resolved.append(override_map[label])
        else:
            resolved.append((label, key_or_fmt, skip))
    resolved.extend(append_list)

    # -- 4. build outputs -------------------------------------------------
    headers = [label for label, _, _ in resolved]

    def auto_row_fn(i, d):
        row = []
        for _label, key_or_fmt, skip in resolved:
            if skip is not None:
                sk, sentinel = skip
                if d.get(sk) == sentinel:
                    row.append("")
                    continue
            if callable(key_or_fmt):
                row.append(key_or_fmt(d))
            else:
                row.append(d.get(key_or_fmt, ""))
        return row

    return (headers, auto_row_fn)

def print_spec_table(
    entries: list,
    spec: dict = None,
    align: str = "l",
    numbered: bool = True,
    col_align: dict = None,
    extra_columns: list = None,
    extra_row_fn: Callable = None,
) -> None:
    """Render a PrettyTable from a Display_Spec.

    Resolves columns from *spec* via :func:`resolve_table_columns`, optionally
    prepends *extra_columns* (used by DumpCache for dynamic fields), then
    delegates to :func:`print_table`.

    If *spec* is not provided, it is read from the first entry's
    ``_display_spec`` attribute.
    """
    if spec is None and entries:
        spec = getattr(entries[0], "_display_spec", None)
    if spec is None:
        print("")
        return
    columns, auto_row_fn = resolve_table_columns(spec)

    if extra_columns and extra_row_fn:
        full_columns = extra_columns + columns

        def combined_row_fn(i, d):
            return extra_row_fn(d) + auto_row_fn(i, d)

        row_fn = combined_row_fn
    else:
        full_columns = columns
        row_fn = auto_row_fn

    print_table(
        entries,
        full_columns,
        row_fn,
        spec["empty_msg"],
        align=align,
        numbered=numbered,
        col_align=col_align,
    )







