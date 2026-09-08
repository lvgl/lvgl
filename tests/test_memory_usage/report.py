#!/usr/bin/env python3
"""
Render the results of measure.py.

Prints the tables as Markdown on stdout, and with --pr-report also emits the
"Memory Usage" section of the shared PR report comment.

    ./report.py --pr results/pr --master results/master \
                --pr-report reports/memory_usage.json

Each of --pr and --master is a directory produced by measure.py: a
`size-results.json` plus a `symbols/` directory. --master is optional; without it
the PR's own numbers are reported with no comparison.
"""

import argparse
import json
import os
import statistics
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "scripts"))

MARKER = "## 📦 Flash / RAM usage"

# A regression bigger than either of these is called out and gets a per-symbol breakdown.
# They are not a gate, only a hint about which rows are worth a look.
NOTABLE_ABS = 2048
NOTABLE_REL = 0.005

# How many symbols to list when a row is worth explaining.
TOP_SYMBOLS = 15

# The whole matrix would otherwise be able to bury the rest of the PR comment.
MAX_SYMBOL_SECTIONS = 3


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


def pct(now, before):
    return (now - before) / before * 100.0 if before else 0.0


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


def collect(pr, master):
    """One record per target and configuration that can be compared with master."""
    rows = []
    for target, configs in pr.items():
        for config, sizes in configs.items():
            base = master.get(target, {}).get(config)
            if not base:
                continue
            lib = sizes.get("library", {}).get("size")
            lib_base = base.get("library", {}).get("size")
            rows.append(
                {
                    "target": target,
                    "config": config,
                    "flash": sizes["flash"],
                    "flash_base": base["flash"],
                    "ram": sizes["ram"],
                    "ram_base": base["ram"],
                    "lib": lib,
                    "lib_base": lib_base,
                }
            )
    return rows


def build_tables(pr, master, pr_symbols, master_symbols):
    """The full matrix, shared by the standalone output and the PR report section."""
    lines = ["### Linked image (`--gc-sections`)", ""]
    if master:
        lines.append("| Target | Config | Flash | master | Δ Flash | RAM | Δ RAM |")
        lines.append("|---|---|---:|---:|---:|---:|---:|")
    else:
        lines.append("| Target | Config | Flash | RAM |")
        lines.append("|---|---|---:|---:|")

    notable = False
    drilldowns = []

    for target, configs in pr.items():
        for config, sizes in configs.items():
            flash_kb = sizes["flash"] / 1000
            ram_kb = sizes["ram"] / 1000
            base = master.get(target, {}).get(config)
            if not master:
                lines.append(
                    f"| {target} | {config} | {flash_kb:.3f} KB | {ram_kb:.3f} KB |"
                )
                continue
            if not base:
                lines.append(
                    f"| {target} | {config} | {flash_kb:.3f} KB | n/a | n/a | "
                    f"{ram_kb:.3f} KB | n/a |"
                )
                continue
            base_flash_kb = base["flash"] / 1000

            mark = is_notable(sizes["flash"], base["flash"])
            notable = notable or mark
            lines.append(
                f"| {target} | {config} | {flash_kb} KB | {base_flash_kb} KB | "
                f"{fmt_delta(sizes['flash'], base['flash'], mark)} | "
                f"{ram_kb} KB | {fmt_delta(sizes['ram'], base['ram'])} |"
            )

            if mark:
                key = f"{target}-{config}"
                body = symbol_table(
                    pr_symbols.get(key, {}), master_symbols.get(key, {})
                )
                if body:
                    drilldowns.append(
                        (sizes["flash"] - base["flash"], target, config, body)
                    )

    lines += ["", "### LVGL library, nothing dead-stripped", ""]
    lines.append(
        "<sub>`liblvgl.a` in full. The linked image above only contains what the "
        "test application reaches. Useful to know how much memory LVGL will require "
        "when using a binding like MicroPython</sub>"
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
            lib_kb = lib / 1000
            base = master.get(target, {}).get(config, {}).get("library", {}).get("size")
            if not master:
                lines.append(f"| {target} | {config} | {lib_kb} KB |")
            elif base is None:
                lines.append(f"| {target} | {config} | {lib_kb} KB | n/a | n/a |")
            else:
                base_kb = base / 1000
                mark = is_notable(lib, base)
                notable = notable or mark
                lines.append(
                    f"| {target} | {config} | {lib_kb} KB | {base_kb} KB | "
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

    drilldowns.sort(key=lambda r: -r[0])
    for d, target, config, body in drilldowns[:MAX_SYMBOL_SECTIONS]:
        lines.append("")
        lines.append(
            f"<details><summary>What grew in "
            f"<code>{target} / {config}</code> ({d:+d} B)</summary>"
        )
        lines.extend(body)
        lines.append("</details>")

    lines += ["", "<sub>Flash is `text + data`, RAM is `data + bss`.</sub>"]
    return lines


def summarise(rows):
    """One line for the shared PR report table.

    Reports the worst target and configuration rather than an average: the failure
    this test exists to catch is a single target running out of flash, and a mean or
    median hides exactly that. The median comes along to say whether the change is
    broad or isolated.
    """
    if not rows:
        return "info", "no baseline to compare against"

    flash_deltas = [r["flash"] - r["flash_base"] for r in rows]
    worst = max(rows, key=lambda r: r["flash"] - r["flash_base"])
    best = min(rows, key=lambda r: r["flash"] - r["flash_base"])
    worst_d = worst["flash"] - worst["flash_base"]
    median = int(statistics.median(flash_deltas))

    lib_rows = [r for r in rows if r["lib"] is not None and r["lib_base"] is not None]
    lib_worst = (
        max(lib_rows, key=lambda r: r["lib"] - r["lib_base"]) if lib_rows else None
    )
    lib_worst_d = lib_worst["lib"] - lib_worst["lib_base"] if lib_worst else 0

    ram_worst = max(rows, key=lambda r: r["ram"] - r["ram_base"])
    ram_worst_d = ram_worst["ram"] - ram_worst["ram_base"]

    if worst_d <= 0 and lib_worst_d <= 0:
        best_d = best["flash"] - best["flash_base"]
        if best_d == 0 and lib_worst_d == 0:
            return "stable", "no change"
        return "down", (
            f"flash {best_d:+d} B ({pct(best['flash'], best['flash_base']):+.1f}%) "
            f"on {best['target']}/{best['config']}"
        )

    notable = is_notable(worst["flash"], worst["flash_base"]) or (
        lib_worst is not None and is_notable(lib_worst["lib"], lib_worst["lib_base"])
    )

    parts = [
        f"flash {worst_d:+d} B ({pct(worst['flash'], worst['flash_base']):+.1f}%) max "
        f"on {worst['target']}/{worst['config']}, {median:+d} B median"
    ]
    # Worth its own mention only when it moved more than the linked image did: that is
    # the case that costs the bindings flash while every linked demo looks unchanged.
    if lib_worst is not None and lib_worst_d > max(worst_d, 0):
        parts.append(f"library {lib_worst_d:+d} B max")
    # Small RAM movements are left to the table: naming them here costs more of the one
    # line than they are worth.
    if is_notable(ram_worst["ram"], ram_worst["ram_base"]):
        parts.append(f"RAM {ram_worst_d:+d} B max")

    return ("warn" if notable else "up"), " · ".join(parts)


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--pr", required=True, help="directory with the PR's results")
    parser.add_argument("--master", help="directory with master's results")
    parser.add_argument(
        "--pr-report",
        help="also write the Memory Usage section of the PR report comment here",
    )
    args = parser.parse_args()

    pr, pr_symbols = load(args.pr)
    master, master_symbols = load(args.master)

    tables = build_tables(pr, master, pr_symbols, master_symbols)
    print("\n".join([MARKER, ""] + tables))

    if args.pr_report:
        from pr_report import write_report

        icon, summary = summarise(collect(pr, master))
        write_report(
            args.pr_report,
            section="Memory Usage",
            icon=icon,
            summary=summary,
            details="\n".join(tables),
        )


if __name__ == "__main__":
    main()
