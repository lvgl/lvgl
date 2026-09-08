#!/usr/bin/env python3
"""
Turn one or two runs of perf_qemu.py into a PR report section.

The counts are exactly reproducible: QEMU under -icount shift=0 is deterministic and
host independent, so any non-zero difference between two runs is a real difference and not
noise. The threshold below only decides which way the row's icon points.

Two things are reported. The instruction counts, per target and suite, against the
baseline. And whether the rendering changed: the reference comparison records a digest of
every rendered scene, so a digest that moved means the PR draws different pixels. That is
reported rather than failed, because a deliberate rendering change is legitimate and only
the author knows which it is.

The baseline is normally whatever master last published, not this branch's merge base, so
the details say which revision it was and complain if it was measured with different
settings. Comparing counts taken at another optimisation level would be meaningless.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "pr_report"))
from pr_report import write_report  # noqa: E402

SECTION = "Render Performance"
ORDER = 31

# Anything smaller than this is real but not worth pointing an icon at
NOTABLE_PCT = 0.5


def load(directory: Path) -> dict:
    """Read every results-<target>.json in a directory, keyed by target."""
    out = {}
    for path in sorted(directory.glob("results-*.json")):
        data = json.loads(path.read_text())
        out[data["target"]] = data
    return out


def settings_of(run: dict) -> dict:
    """The settings every target in a run shares, if they agree."""
    seen = [d.get("settings", {}) for d in run.values()]
    return seen[0] if seen and all(x == seen[0] for x in seen) else {}


def provenance(new: dict, base: dict) -> tuple[list[str], bool]:
    """What was measured and what it was compared against.

    Returns the lines to show and whether the baseline's settings make the comparison
    meaningless.
    """
    a, b = settings_of(new), settings_of(base)
    lines = []
    mismatch = False
    if a:
        lines.append(f"- Measured at `-{a.get('opt')}`, colour formats "
                     f"{', '.join(a.get('formats', []))}.")
    if b:
        head = b.get("tree") or ""
        lines.append(f"- Baseline: `{head[:12]}`." if head
                     else "- Baseline: an unidentified revision, possibly a modified tree.")
        # Only the settings that change a number; the revision is expected to differ
        differing = [k for k in ("opt", "formats", "suites", "tolerance")
                     if a.get(k) != b.get(k)]
        if differing:
            mismatch = True
            lines.append(f"- **The baseline used different settings "
                         f"({', '.join(differing)}), so the changes below do not mean "
                         f"anything.** Republish the baseline from master.")
    return lines, mismatch


def pct(new: float, base: float) -> float | None:
    return None if not base else (new - base) / base * 100.0


def fmt_pct(value: float | None) -> str:
    if value is None:
        return "-"
    return f"{value:+.2f}%"


def totals_table(new: dict, base: dict) -> tuple[str, list[float]]:
    rows = ["| Target | Suite | Instructions | Baseline | Change |", "|---|---|--:|--:|--:|"]
    changes = []
    for target in sorted(new):
        for suite in sorted(new[target]["suites"]):
            n = new[target]["suites"][suite]["total"]
            b = base.get(target, {}).get("suites", {}).get(suite, {}).get("total")
            change = pct(n, b) if b else None
            if change is not None:
                changes.append(change)
            rows.append(f"| {target} | {suite} | {n:,} | "
                        f"{f'{b:,}' if b else '-'} | {fmt_pct(change)} |")
    return "\n".join(rows), changes


def movers_table(new: dict, base: dict, limit: int) -> str:
    """The per-scene changes, largest first in either direction."""
    entries = []
    for target in sorted(new):
        for suite, data in new[target]["suites"].items():
            b_scenes = base.get(target, {}).get("suites", {}).get(suite, {}).get("scenes", {})
            for scene, value in data["scenes"].items():
                if scene not in b_scenes:
                    continue
                change = pct(value, b_scenes[scene])
                if not change:
                    continue  # a scene that did not move is not a mover
                entries.append((abs(change), change, target, suite, scene, value,
                                b_scenes[scene]))
    if not entries:
        return ""
    entries.sort(reverse=True)
    rows = ["| Target | Suite | Scene | Instructions | Baseline | Change |",
            "|---|---|---|--:|--:|--:|"]
    for _, change, target, suite, scene, value, b in entries[:limit]:
        rows.append(f"| {target} | {suite} | {scene} | {value:,} | {b:,} | {fmt_pct(change)} |")
    return "\n".join(rows)


def scene_digests(run: dict) -> dict:
    """(target, colour format, scene) -> digest, for every render run that compared."""
    out = {}
    for target, data in run.items():
        for suite, sdata in data["suites"].items():
            refs = sdata.get("refs")
            if not refs:
                continue
            cf = suite.replace("render_", "")
            for scene, info in refs["scenes"].items():
                out[(target, cf, scene)] = info["digest"]
    return out


def render_changes(new: dict, base: dict) -> list[str]:
    a, b = scene_digests(new), scene_digests(base)
    return sorted(f"{key[0]} {key[1]} {key[2]}" for key, digest in a.items()
                  if key in b and b[key] != digest)


def ref_deltas(new: dict) -> list[str]:
    """Scenes that do not match the committed reference images."""
    out = []
    for target, data in new.items():
        for suite, sdata in data["suites"].items():
            refs = sdata.get("refs")
            if not refs:
                continue
            cf = suite.replace("render_", "")
            for scene in refs["differing"]:
                info = refs["scenes"][scene]
                out.append(f"{target} {cf} {scene}: max delta {info['max_delta']}, "
                           f"{info['bad_px']:,} pixels")
            for scene in refs["no_ref"]:
                out.append(f"{target} {cf} {scene}: reference {refs['scenes'][scene]['ref']}")
    return sorted(out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--new", required=True, type=Path,
                    help="directory of results-<target>.json for the PR")
    ap.add_argument("--base", type=Path,
                    help="the same for the baseline; without it only absolute numbers "
                         "are reported")
    ap.add_argument("--output", required=True, type=Path, help="the report JSON to write")
    ap.add_argument("--movers", type=int, default=25,
                    help="how many per-scene changes to list")
    args = ap.parse_args()

    new = load(args.new)
    if not new:
        print(f"no results-*.json in {args.new}", file=sys.stderr)
        return 1
    base = load(args.base) if args.base else {}

    totals, changes = totals_table(new, base)
    changed_scenes = render_changes(new, base) if base else []
    refs = ref_deltas(new)

    prov, prov_mismatch = provenance(new, base)
    parts = (prov + [""] if prov else []) + ["### Instruction counts", "", totals]
    if base:
        movers = movers_table(new, base, args.movers)
        if movers:
            parts += ["", f"### Largest per-scene changes (top {args.movers})", "", movers]
    if changed_scenes:
        parts += ["", "### Rendering changed", "",
                  "These scenes no longer render the same bytes as the baseline. That is "
                  "fine if it was intended; if it was not, it is a rendering regression.",
                  ""]
        parts += [f"- {s}" for s in changed_scenes]
    if refs:
        parts += ["", "### Differences from the committed reference images", "",
                  "`tests/ref_imgs/draw/render`, compared at tolerance 0.", ""]
        parts += [f"- {s}" for s in refs]

    if not base:
        icon = "warn" if refs else "info"
        summary = f"{len(new)} target(s) measured, no baseline to compare against"
    elif not changes:
        # A baseline was given but nothing in it lined up with this run, so there is no
        # comparison to report. Saying "+0.00%" here would be a lie.
        icon = "warn"
        summary = ("a baseline was given but none of its targets or suites match this "
                   "run, so nothing was compared")
    else:
        worst, best = max(changes), min(changes)
        if best <= -NOTABLE_PCT and worst < NOTABLE_PCT:
            icon = "down"
        elif worst >= NOTABLE_PCT:
            icon = "up"
        else:
            icon = "stable"
        summary = f"{fmt_pct(best)} to {fmt_pct(worst)} across {len(new)} target(s)"
        if prov_mismatch:
            icon = "warn"
        if changed_scenes:
            icon = "warn"
            summary += f"; {len(changed_scenes)} render scene(s) changed"
        else:
            summary += "; rendering unchanged"
    if refs:
        summary += f"; {len(refs)} scene(s) differ from the reference images"

    write_report(args.output, section=SECTION, icon=icon, summary=summary,
                 details="\n".join(parts), order=ORDER)
    print(summary)
    return 0


if __name__ == "__main__":
    sys.exit(main())
