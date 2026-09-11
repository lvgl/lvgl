#!/usr/bin/env python3
"""Regenerate COPYRIGHTS.md from the single source of truth.

Component data lives in sbom/third_party.json, which is also used to
generate the SPDX SBOM (see generate_sbom.py). Edit that JSON, then run this
script to refresh COPYRIGHTS.md.

Usage:
    python3 scripts/generate_copyrights.py [--output COPYRIGHTS.md] [--check]

    --check  exit non-zero if COPYRIGHTS.md is out of date (for CI).
"""

import argparse
import json
import os
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_FILE = os.path.join(REPO_ROOT, "sbom", "third_party.json")

HEADER = (
    "LVGL uses the following third-party libraries.\n"
    "Each entry lists its SPDX license expression below. The full license text "
    "is, for most libraries, found in the corresponding library folder (see the "
    "paths); some libraries only reference their license rather than shipping "
    "the full text. A machine-readable inventory is available in `sbom/`.\n"
)


def render(data):
    lines = [HEADER]
    for comp in data["components"]:
        lines.append("**%s**" % comp["name"])

        paths = comp.get("paths", [])
        if len(paths) == 1:
            lines.append("- Path: %s" % paths[0])
        elif paths:
            lines.append("- Path:")
            for p in paths:
                lines.append("    - %s" % p)

        sources = comp.get("sources", [])
        if len(sources) == 1 and not sources[0].get("note"):
            lines.append("- Source: %s" % sources[0]["url"])
        elif sources:
            lines.append("- Source:")
            for s in sources:
                suffix = " (*%s*)" % s["note"] if s.get("note") else ""
                lines.append("    - %s%s" % (s["url"], suffix))

        if comp.get("license"):
            lines.append("- License: %s" % comp["license"])

        if comp.get("note"):
            lines.append("- Note: %s" % comp["note"])

        lines.append("")  # blank line between entries

    return "\n".join(lines).rstrip() + "\n"


def main():
    parser = argparse.ArgumentParser(description="Regenerate COPYRIGHTS.md.")
    parser.add_argument("--output", default=os.path.join(REPO_ROOT, "COPYRIGHTS.md"),
                        help="Output file (default: COPYRIGHTS.md).")
    parser.add_argument("--check", action="store_true",
                        help="Exit non-zero if the output is out of date.")
    args = parser.parse_args()

    with open(DATA_FILE, encoding="utf-8") as fh:
        data = json.load(fh)
    content = render(data)

    if args.check:
        current = ""
        if os.path.isfile(args.output):
            with open(args.output, encoding="utf-8") as fh:
                current = fh.read()
        if current != content:
            print("%s is out of date; run scripts/generate_copyrights.py" % args.output)
            return 1
        print("%s is up to date" % args.output)
        return 0

    with open(args.output, "w", encoding="utf-8") as fh:
        fh.write(content)
    print("Wrote %s (%d components)" % (args.output, len(data["components"])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
