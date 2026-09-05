#!/usr/bin/env python3
"""
Render the results of measure.py as a Markdown table for a pull request comment.
"""

import argparse
import json
import os

MARKER = "## 📦 Flash / RAM usage"

# A regression bigger than either of these is called out and gets a per-symbol breakdown.
# They are not a gate, only a hint about which rows are worth a look.
NOTABLE_ABS = 2048
NOTABLE_REL = 0.005

# How many symbols to list when a row is worth explaining.
TOP_SYMBOLS = 15


def load(directory):
    if not directory:
        return {}, {}
    try:
        with open(os.path.join(directory, "size-results.json")) as f:
            totals = json.load(f)
    except (OSError, json.JSONDecodeError):
        return {}, {}

    symbols = {}
    symbols_dir = os.path.join(directory, "symbols")
    if os.path.isdir(symbols_dir):
        for name in os.listdir(symbols_dir):
            if name.endswith(".json"):
                with open(os.path.join(symbols_dir, name)) as f:
                    symbols[name[: -len(".json")]] = json.load(f)
    return totals, symbols


def is_notable(now, before):
    d = now - before
    return d >= NOTABLE_ABS or (before > 0 and d / before >= NOTABLE_REL)


def fmt_delta(now, before, mark=False):
    d = now - before
    text = f"{d:+d} B"
    return f"⚠️ {text}" if mark else text


def symbol_table(pr_symbols, master_symbols):
    """The symbols that moved most, so a regression says where and not just how much."""
    names = set(pr_symbols) | set(master_symbols)
    rows = sorted(
        ((pr_symbols.get(n, 0) - master_symbols.get(n, 0), n) for n in names),
        key=lambda r: -abs(r[0]),
    )
    rows = [r for r in rows if r[0]][:TOP_SYMBOLS]
    if not rows:
        return []

    lines = ["", "| Symbol | master | PR | Δ |", "|---|---:|---:|---:|"]
    for d, name in rows:
        before = master_symbols.get(name, 0)
        now = pr_symbols.get(name, 0)
        lines.append(f"| `{name}` | {before} B | {now} B | {d:+d} B |")
    lines.append("")
    return lines


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--pr", required=True, help="directory with the PR's results")
    parser.add_argument("--master", help="directory with master's results")
    args = parser.parse_args()

    pr, pr_symbols = load(args.pr)
    master, master_symbols = load(args.master)

    lines = [MARKER, ""]
    details = []
    notable = False

    lines.append("### Linked image (`--gc-sections`)")
    lines.append("")
    if master:
        lines.append("| Target | Config | Flash | master | Δ Flash | RAM | Δ RAM |")
        lines.append("|---|---|---:|---:|---:|---:|---:|")
    else:
        lines.append("| Target | Config | Flash | RAM |")
        lines.append("|---|---|---:|---:|")

    for target, configs in pr.items():
        for config, sizes in configs.items():
            base = master.get(target, {}).get(config)
            if not master:
                lines.append(
                    f"| {target} | {config} | {sizes['flash']} B | {sizes['ram']} B |"
                )
                continue
            if not base:
                lines.append(
                    f"| {target} | {config} | {sizes['flash']} B | n/a | n/a | "
                    f"{sizes['ram']} B | n/a |"
                )
                continue

            mark = is_notable(sizes["flash"], base["flash"])
            notable = notable or mark
            lines.append(
                f"| {target} | {config} | {sizes['flash']} B | {base['flash']} B | "
                f"{fmt_delta(sizes['flash'], base['flash'], mark)} | "
                f"{sizes['ram']} B | {fmt_delta(sizes['ram'], base['ram'])} |"
            )

            if mark:
                key = f"{target}-{config}"
                body = symbol_table(
                    pr_symbols.get(key, {}), master_symbols.get(key, {})
                )
                if body:
                    d = sizes["flash"] - base["flash"]
                    details.append(
                        f"<details><summary>What grew in "
                        f"<code>{target} / {config}</code> ({d:+d} B)</summary>"
                    )
                    details.extend(body)
                    details.append("</details>")
                    details.append("")

    lines.append("")
    lines.append("### LVGL library, nothing dead-stripped")
    lines.append("")
    lines.append(
        "<sub>`liblvgl.a` in full. The linked image above only contains what the "
        "test application reaches; bindings such as MicroPython's reference every "
        "public symbol, so for them nothing is ever stripped and this is the number "
        "that matters.</sub>"
    )
    lines.append("")
    if master:
        lines.append("| Target | Config | Library | master | Δ |")
        lines.append("|---|---|---:|---:|---:|")
    else:
        lines.append("| Target | Config | Library |")
        lines.append("|---|---|---:|")

    for target, configs in pr.items():
        for config, sizes in configs.items():
            lib = sizes.get("library", {}).get("size")
            if lib is None:
                continue
            base = master.get(target, {}).get(config, {}).get("library", {}).get("size")
            if not master:
                lines.append(f"| {target} | {config} | {lib} B |")
            elif base is None:
                lines.append(f"| {target} | {config} | {lib} B | n/a | n/a |")
            else:
                mark = is_notable(lib, base)
                notable = notable or mark
                lines.append(
                    f"| {target} | {config} | {lib} B | {base} B | "
                    f"{fmt_delta(lib, base, mark)} |"
                )

    lines.append("")
    if not master:
        lines.append("_No master baseline was available for comparison._")
    elif notable:
        lines.append(
            f"⚠️ marks an increase of at least {NOTABLE_ABS} B or {NOTABLE_REL:.1%}. "
            "That is a hint, not a failure: a deliberate trade is fine, it just "
            "deserves a sentence in the PR description."
        )

    if details:
        lines.append("")
        lines.extend(details)

    lines.append("<sub>Flash is `text + data`, RAM is `data + bss`.</sub>")

    print("\n".join(lines))


if __name__ == "__main__":
    main()
