"""A small ASCII table renderer.

It draws the same boxes prettytable does by default, so the plugin does not
need prettytable installed in the Python that GDB embeds:

    +---+-------+
    | # | name  |
    +---+-------+
    | 0 | timer |
    +---+-------+

Cells may hold several lines, and wide (CJK) characters count as two columns.
"""

import unicodedata
from typing import Dict, List, Sequence


def _char_width(ch: str) -> int:
    if unicodedata.combining(ch) or unicodedata.category(ch) in ("Mn", "Me", "Cf"):
        return 0
    if unicodedata.east_asian_width(ch) in ("W", "F"):
        return 2
    return 1


def text_width(text: str) -> int:
    """Width of one line of text in terminal columns."""
    if text.isascii():
        return len(text)
    return sum(_char_width(ch) for ch in text)


def _justify(text: str, width: int, align: str) -> str:
    excess = width - text_width(text)
    if align == "l":
        return text + " " * excess
    if align == "r":
        return " " * excess + text
    # Centre the way str.center() does: the odd space goes to the right of
    # odd-length text and to the left of even-length text.
    left = excess // 2
    if excess % 2 and text_width(text) % 2 == 0:
        left += 1
    return " " * left + text + " " * (excess - left)


class TextTable:
    """Collects rows and renders them as an ASCII table.

    `align` is the default alignment of every column ("l", "c" or "r");
    `col_align` overrides it per column name.
    """

    def __init__(self, field_names: Sequence[str], align: str = "c",
                 col_align: Dict[str, str] = None):
        self.field_names = [str(name) for name in field_names]
        self.align = {name: align for name in self.field_names}
        for name, a in (col_align or {}).items():
            if name in self.align:
                self.align[name] = a
        self.rows: List[List[str]] = []

    def add_row(self, row: Sequence) -> None:
        if len(row) != len(self.field_names):
            raise ValueError(
                f"Row has {len(row)} cells, but the table has"
                f" {len(self.field_names)} columns"
            )
        self.rows.append([str(cell) for cell in row])

    def __str__(self) -> str:
        names = self.field_names
        widths = [text_width(name) for name in names]
        for row in self.rows:
            for i, cell in enumerate(row):
                for line in cell.split("\n"):
                    widths[i] = max(widths[i], text_width(line))

        rule = "+" + "+".join("-" * (w + 2) for w in widths) + "+"

        def render(cells: Sequence[str]) -> List[str]:
            split = [cell.split("\n") for cell in cells]
            height = max(len(lines) for lines in split)
            out = []
            for n in range(height):
                parts = []
                for i, lines in enumerate(split):
                    text = lines[n] if n < len(lines) else ""
                    parts.append(_justify(text, widths[i], self.align[names[i]]))
                out.append("| " + " | ".join(parts) + " |")
            return out

        lines = [rule, *render(names), rule]
        if self.rows:
            for row in self.rows:
                lines.extend(render(row))
            lines.append(rule)
        return "\n".join(lines)
