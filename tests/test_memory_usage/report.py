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

KB = 1024

# The whole matrix would otherwise be able to bury the rest of the PR comment.
MAX_SYMBOL_SECTIONS = 3


def load(directory):
    if not directory:
        return {}, {}, ""
    try:
        with open(os.path.join(directory, "size-results.json")) as f:
            totals = json.load(f)
    except (OSError, json.JSONDecodeError):
        return {}, {}, ""

    try:
        with open(os.path.join(directory, "commit.txt")) as f:
            commit = f.read().strip()[:9]
    except OSError:
        commit = ""

    symbols = {}
    symbols_dir = os.path.join(directory, "symbols")
    if os.path.isdir(symbols_dir):
        for name in os.listdir(symbols_dir):
            if name.endswith(".json"):
                with open(os.path.join(symbols_dir, name)) as f:
                    symbols[name[: -len(".json")]] = json.load(f)
    return totals, symbols, commit


def fmt_kb(value):
    return f"{value / KB:.3f} KB"


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


def build_tables(pr, master, pr_symbols, master_symbols, pr_commit="", master_commit=""):
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
            base = master.get(target, {}).get(config)
            if not master:
                lines.append(
                    f"| {target} | {config} | {fmt_kb(sizes['flash'])} | "
                    f"{fmt_kb(sizes['ram'])} |"
                )
                continue
            if not base:
                lines.append(
                    f"| {target} | {config} | {fmt_kb(sizes['flash'])} | n/a | n/a | "
                    f"{fmt_kb(sizes['ram'])} | n/a |"
                )
                continue

            mark = is_notable(sizes["flash"], base["flash"])
            notable = notable or mark
            lines.append(
                f"| {target} | {config} | {fmt_kb(sizes['flash'])} | "
                f"{fmt_kb(base['flash'])} | "
                f"{fmt_delta(sizes['flash'], base['flash'], mark)} | "
                f"{fmt_kb(sizes['ram'])} | {fmt_delta(sizes['ram'], base['ram'])} |"
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
            base = master.get(target, {}).get(config, {}).get("library", {}).get("size")
            if not master:
                lines.append(f"| {target} | {config} | {fmt_kb(lib)} |")
            elif base is None:
                lines.append(f"| {target} | {config} | {fmt_kb(lib)} | n/a | n/a |")
            else:
                mark = is_notable(lib, base)
                notable = notable or mark
                lines.append(
                    f"| {target} | {config} | {fmt_kb(lib)} | {fmt_kb(base)} | "
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

    lines += ["", "<sub>Flash is `text + data`, RAM is `data + bss`."]
    if pr_commit and master_commit:
        # The baseline is the base branch's last successful run, which may be behind its
        # tip. Naming both commits makes a stale comparison visible.
        lines.append(f"Comparing `{pr_commit}` against `{master_commit}`.</sub>")
    elif pr_commit:
        lines.append(f"Measured at `{pr_commit}`.</sub>")
    else:
        lines[-1] += "</sub>"
    return lines


def metric_summary(rows, key, label):
    """One clause for one metric: what it did, and the target where it did it worst.

    Returns the clause, the delta it describes and whether that delta is notable.
    """
    base_key = f"{key}_base"
    comparable = [r for r in rows if r[key] is not None and r[base_key] is not None]
    if not comparable:
        return "", 0, False

    worst = max(comparable, key=lambda r: r[key] - r[base_key])
    best = min(comparable, key=lambda r: r[key] - r[base_key])
    grew = worst[key] - worst[base_key]
    shrank = best[key] - best[base_key]

    if grew == 0 and shrank == 0:
        return f"{label} unchanged", 0, False

    row, delta = (worst, grew) if grew > 0 else (best, shrank)
    return (
        f"{label} {delta:+d} B ({pct(row[key], row[base_key]):+.1f}%) on "
        f"{row['target']}/{row['config']}",
        delta,
        is_notable(worst[key], worst[base_key]),
    )


def summarise(rows):
    """One line for the shared PR report table.

    Every metric reports itself. Picking one to headline is what used to make this
    claim things about the metric it had not picked - "flash unchanged" on a change
    that shrank flash, for one.

    Each clause names the worst target and configuration rather than an average: the
    failure this test exists to catch is a single target running out of flash, and a
    mean or a median hides exactly that. The median comes along for flash because it
    says for free whether a change is broad or isolated.
    """
    if not rows:
        return "info", "no baseline to compare against"

    flash, flash_delta, flash_notable = metric_summary(rows, "flash", "flash")
    ram, ram_delta, ram_notable = metric_summary(rows, "ram", "RAM")
    lib, lib_delta, lib_notable = metric_summary(rows, "lib", "library")

    if not flash_delta and not ram_delta and not lib_delta:
        return "stable", "no change"

    parts = [flash, ram]
    if flash_delta:
        median = int(statistics.median(r["flash"] - r["flash_base"] for r in rows))
        parts[0] += f", {median:+d} B median"

    # Worth its own clause when it says something flash does not: either it grew more
    # than the linked image did - the case that costs the bindings flash while every
    # linked demo looks unchanged - or it is the only thing that moved at all.
    if lib and (lib_delta > max(flash_delta, 0) or not (flash_delta or ram_delta)):
        parts.append(lib)

    if flash_notable or ram_notable or lib_notable:
        icon = "warn"
    elif max(flash_delta, ram_delta, lib_delta) > 0:
        icon = "up"
    else:
        icon = "down"

    return icon, " · ".join(p for p in parts if p)


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

    pr, pr_symbols, pr_commit = load(args.pr)
    master, master_symbols, master_commit = load(args.master)

    tables = build_tables(
        pr, master, pr_symbols, master_symbols, pr_commit, master_commit
    )
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
