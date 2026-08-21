#!/usr/bin/env python3
"""
Shared report format for the single PR comment.

  emit    Write a report file (for shell-based workflows)
  render  Turn a directory of report files into the comment body

A report:

  {
    "section": "API",            # the row label, unique per workflow
    "order":   30,               # optional, row order (default 50)
    "icon":    "white_check_mark",
    "summary": "no change",      # one short line
    "details": "..."             # optional markdown, shown in a <details> block
  }

`icon` can be a name from ICONS below or a raw emoji
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

MARKER = "<!-- lvgl-pr-report -->"

ICONS = {
    "ok": "✅",
    "fail": "❌",
    "warn": "⚠️",
    "up": "📈",
    "down": "📉",
    "stable": "➖",
    "info": "ℹ️",
    "pending": "⏳",
    "skip": "⏭️",
}

# Rows that are expected to appear. Anything present but not listed is appended;
# anything listed but missing renders as pending, so a reader can tell "not run
# yet" from "nothing to say".
EXPECTED_SECTIONS = [
    ("Static Checks", 10),
    ("Coverage", 20),
    ("Performance", 30),
]


def icon_for(name: str) -> str:
    return ICONS.get(name, name)


def write_report(
    path: Path | str,
    section: str,
    icon: str,
    summary: str,
    details: str = "",
    order: int | None = None,
) -> None:
    if order is None:
        order = dict(EXPECTED_SECTIONS).get(section, 50)
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        json.dumps(
            {
                "section": section,
                "order": order,
                "icon": icon_for(icon),
                "summary": summary,
                "details": details,
            },
            indent=1,
        )
        + "\n"
    )


def load_reports(directory: Path) -> list[dict]:
    reports: dict[str, dict] = {}
    for path in sorted(directory.rglob("*.json")):
        try:
            data = json.loads(path.read_text())
        except (OSError, json.JSONDecodeError):
            continue
        if not isinstance(data, dict) or "section" not in data:
            continue
        data.setdefault("order", 50)
        data.setdefault("icon", ICONS["info"])
        data.setdefault("summary", "")
        data.setdefault("details", "")
        # last one wins if a workflow uploaded twice
        reports[data["section"]] = data
    return sorted(reports.values(), key=lambda r: (r["order"], r["section"]))


def render(reports: list[dict], *, show_pending: bool = True) -> str:
    seen = {r["section"] for r in reports}
    rows = list(reports)
    if show_pending:
        for name, order in EXPECTED_SECTIONS:
            if name not in seen:
                rows.append(
                    {
                        "section": name,
                        "order": order,
                        "icon": ICONS["pending"],
                        "summary": "pending",
                        "details": "",
                    }
                )
        rows.sort(key=lambda r: (r["order"], r["section"]))

    lines = [MARKER, "", "## PR report", "", "| | Check | Result |", "|:-:|---|---|"]
    for r in rows:
        summary = " ".join(str(r["summary"]).split()) or "-"
        lines.append(f"| {r['icon']} | {r['section']} | {summary} |")

    for r in rows:
        details = (r.get("details") or "").strip()
        if not details:
            continue
        lines += [
            "",
            f"<details><summary>{r['section']} — {' '.join(str(r['summary']).split())}</summary>",
            "",
            details,
            "",
            "</details>",
        ]

    lines += [
        "",
        "<sub>Generated automatically. One comment, updated as each "
        "workflow finishes.</sub>",
    ]
    return "\n".join(lines) + "\n"


def cmd_emit(args) -> int:
    details = args.details
    if args.details_file:
        p = Path(args.details_file)
        details = p.read_text() if p.is_file() else ""
    write_report(
        Path(args.output),
        section=args.section,
        icon=args.icon,
        summary=args.summary,
        details=details or "",
        order=args.order,
    )
    print(f"wrote {args.output}")
    return 0


def cmd_render(args) -> int:
    reports = load_reports(Path(args.input))
    body = render(reports, show_pending=not args.no_pending)
    if args.output:
        Path(args.output).write_text(body)
        print(f"wrote {args.output} ({len(reports)} section(s))")
    else:
        sys.stdout.write(body)
    return 0


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[1])
    sub = parser.add_subparsers(dest="cmd", required=True)

    e = sub.add_parser("emit", help="write a report file")
    e.add_argument("--output", required=True)
    e.add_argument("--section", required=True)
    e.add_argument(
        "--icon", required=True, help=f"one of {', '.join(ICONS)} or an emoji"
    )
    e.add_argument("--summary", required=True)
    e.add_argument("--details", default="")
    e.add_argument("--details-file", default="")
    e.add_argument("--order", type=int, default=None)
    e.set_defaults(func=cmd_emit)

    r = sub.add_parser("render", help="render a directory of reports")
    r.add_argument("--input", required=True)
    r.add_argument("--output", default="")
    r.add_argument(
        "--no-pending",
        action="store_true",
        help="omit rows for sections that did not report",
    )
    r.set_defaults(func=cmd_render)

    args = parser.parse_args()
    sys.exit(args.func(args))


if __name__ == "__main__":
    main()
