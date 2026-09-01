#!/usr/bin/env python3
"""Run Zephyr's LVGL samples built by twister and report the outcome.

    samples.py run    --outdir twister-out --results results.json [--seconds 20]
    samples.py report --outdir twister-out --results results.json \
                      --output pr-report/zephyr.json [--details details.md]

``run`` executes every ``zephyr.exe`` that twister produced.  The samples are
GUI applications with no end condition, so "it works" means "it kept running":
each one is given *seconds* to run and is then stopped with SIGINT.

    exit 124 (our timeout fired)  -> the sample ran the whole window: pass
    exit 0                        -> it exited on its own before the window
                                     was up, which a GUI sample should not do
    anything else                 -> crash (SIGSEGV is 139), assert, or abort

``report`` combines the twister build results with the run results and writes
them, through ``scripts/pr_report``, into the report artifact that
.github/workflows/pr_comment.yml renders into the single PR comment.
Neither subcommand fails on a broken sample; the workflow decides that from the
counts in the results file.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import os
import subprocess
import sys
from pathlib import Path

# native_sim's display driver is SDL. 'dummy' alone leaves SDL without a
# renderer ("Couldn't find matching render driver"), so the software renderer
# has to be asked for explicitly. With both set no X server or xvfb is needed.
HEADLESS_ENV = {"SDL_VIDEODRIVER": "dummy", "SDL_RENDER_DRIVER": "software"}

TIMED_OUT = 124  # coreutils timeout(1) exit code when it had to kill the child

# Icon names, translated to emoji by pr_report.write_report(). Do not write the
# emoji here: the renderer prints the stored `icon` field verbatim, so
# translating is write_report()'s job and duplicating it would let the two
# drift.
ICON_OK, ICON_FAIL, ICON_WARN = "ok", "fail", "warn"

SECTION = "Zephyr"
SECTION_ORDER = 40


def pr_report():
    """The shared report writer, ``scripts/pr_report/pr_report.py``.

    Loaded by path rather than imported because ``scripts`` is not a package.
    Returns None when it is not in the tree, in which case this check still
    builds and runs the samples but contributes no row to the PR comment. Never
    reimplement the format here: it is defined in one place, and the icon has to
    be translated by ``write_report()``.
    """
    path = Path(__file__).resolve().parents[1] / "pr_report" / "pr_report.py"
    if not path.is_file():
        return None
    spec = importlib.util.spec_from_file_location("lvgl_pr_report", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def write_pr_report(out: Path, icon: str, summary: str, details: str) -> bool:
    """Write the report artifact .github/workflows/pr_comment.yml collects.

    Returns False, having logged why, when the shared writer is not available.
    That is not a failure of this check: the verdict is reported separately, so
    only the PR comment row is missing.
    """
    lib = pr_report()
    if lib is None:
        note = ("scripts/pr_report is not in this tree, so no PR comment row "
                "was written for the Zephyr check. The build and run result "
                "above still stands.")
        if os.environ.get("GITHUB_ACTIONS"):
            print(f"::warning::{note}")
        print(f"note: {note}")
        return False
    lib.write_report(out, section=SECTION, icon=icon, summary=summary,
                     details=details, order=SECTION_ORDER)
    return True


def write_verdict(path: str, icon: str, summary: str) -> None:
    """Record the pass/fail verdict for the workflow's final gate.

    Deliberately separate from the PR report: whether this check passes must not
    depend on the comment machinery being present or working.
    """
    if not path:
        return
    out = Path(path)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({"icon": icon, "summary": summary}, indent=1) + "\n")


# --------------------------------------------------------------------------
# run
# --------------------------------------------------------------------------
def find_executables(outdir: Path) -> list[tuple[str, Path]]:
    """(test name, executable) for every sample twister actually built.

    Located by globbing rather than by rebuilding twister's output layout from
    twister.json, so a change to that layout does not silently find nothing.
    The test name is the directory holding the ``zephyr`` build output.
    """
    return [(exe.parent.parent.name, exe)
            for exe in sorted(outdir.rglob("zephyr/zephyr.exe"))]


def run_one(name: str, exe: Path, seconds: int) -> dict:
    env = dict(os.environ, **HEADLESS_ENV)
    cmd = ["timeout", "-s", "INT", str(seconds), "./" + exe.name]
    try:
        proc = subprocess.run(
            cmd, cwd=exe.parent, env=env, capture_output=True, text=True,
            errors="replace", timeout=seconds + 60,
        )
        code, out = proc.returncode, proc.stdout + proc.stderr
    except subprocess.TimeoutExpired:
        # timeout(1) itself did not return; treat as a hang, not a pass
        return {"name": name, "exit_code": None, "status": "hang", "output": "",
                "note": f"did not terminate within {seconds + 60}s"}

    if code == TIMED_OUT:
        status, note = "pass", f"ran for {seconds}s without crashing"
    elif code == 0:
        status, note = "fail", "exited on its own before the run window was up"
    elif code == 128 + 11:
        status, note = "fail", "segmentation fault"
    elif code > 128:
        status, note = "fail", f"killed by signal {code - 128}"
    else:
        status, note = "fail", f"exited with code {code}"
    return {"name": name, "exit_code": code, "status": status, "note": note,
            "output": out[-4000:]}


def cmd_run(args) -> int:
    outdir = Path(args.outdir)
    executables = find_executables(outdir)
    results = []
    for name, exe in executables:
        res = run_one(name, exe, args.seconds)
        results.append(res)
        print(f"{res['status']:5s} {name} ({res['note']})", flush=True)

    payload = {"seconds": args.seconds, "samples": results}
    Path(args.results).parent.mkdir(parents=True, exist_ok=True)
    Path(args.results).write_text(json.dumps(payload, indent=1) + "\n")

    failed = [r for r in results if r["status"] != "pass"]
    print(f"\n{len(results) - len(failed)}/{len(results)} samples ran clean")
    return 0


# --------------------------------------------------------------------------
# report
# --------------------------------------------------------------------------
def load_twister(outdir: Path) -> list[dict]:
    path = outdir / "twister.json"
    if not path.is_file():
        return []
    try:
        return json.loads(path.read_text()).get("testsuites", [])
    except (OSError, json.JSONDecodeError):
        return []


def source_remaps(outdir: Path) -> list[str]:
    """What zephyr-with-custom-lvgl.cmake had to reconcile.

    The hook writes the same list into every sample's build directory, so the
    first non-empty one is representative. This is Zephyr's stale LVGL source
    list expressed as data: it is what we still owe upstream.
    """
    for path in sorted(outdir.rglob("lvgl_source_remap.txt")):
        try:
            lines = [ln.strip() for ln in path.read_text().splitlines() if ln.strip()]
        except OSError:
            continue
        if lines:
            return lines
    return []


def build_failures(suites: list[dict]) -> list[dict]:
    """Suites twister could not build. 'filtered' is not a failure -- it means
    the sample does not apply to this board (needs a shield, a sensor, ...)."""
    bad = []
    for s in suites:
        status = (s.get("status") or "").lower()
        if status in ("error", "failed"):
            bad.append(s)
    return bad


def expected_to_run(suites: list[dict]) -> list[str]:
    """Names of suites that produced a binary and therefore must have a run
    result. A suite that failed to build, or that twister filtered out for this
    board, is not expected to run."""
    return [s["name"] for s in suites
            if (s.get("status") or "").lower() not in ("error", "failed", "filtered")]


def repro_block(name: str, path: str, board: str) -> str:
    return (
        f"Reproduce `{name}` locally (see `tests/zephyr/README.md` for the "
        "workspace setup):\n\n"
        "```sh\n"
        f"west twister -p {board} -s {name} --fixture fixture_display\n"
        "# or build it directly:\n"
        f"west build -p always -b {board} zephyr/{path} -T {name}\n"
        "# and run it headless the way CI does:\n"
        "cd build/zephyr && SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software \\\n"
        "  timeout -s INT 20 ./zephyr.exe\n"
        "```\n"
    )


def render_details(suites: list[dict], runs: list[dict], board: str, pin: str,
                   version_note: str, patches: list[str],
                   remaps: list[str]) -> str:
    lines = [f"Zephyr `{pin}`, board `{board}`.", ""]

    if version_note:
        lines += [version_note, ""]

    if remaps:
        lines += [
            f"`tests/zephyr/zephyr-with-custom-lvgl.cmake` reconciled "
            f"{len(remaps)} LVGL source path(s) that Zephyr's own module list "
            "still has at their old locations. No Zephyr patch is needed for "
            "these, but they are what Zephyr's list owes LVGL's layout:",
            "",
            "<details><summary>reconciled source paths</summary>",
            "",
            "```",
            *remaps,
            "```",
            "",
            "</details>",
            "",
        ]

    if patches:
        lines += [
            f"{len(patches)} local patch(es) were applied to the Zephyr tree "
            "before building. These are fixes that are not upstream yet — see "
            "`tests/zephyr/patches/`:",
            "",
        ]
        lines += [f"- `{p}`" for p in patches]
        lines += [""]

    filtered = [s for s in suites if (s.get("status") or "").lower() == "filtered"]
    failures = build_failures(suites)

    lines += [
        "| | Sample | Build | Run |",
        "|:-:|---|---|---|",
    ]
    run_by_name = {r["name"]: r for r in runs}
    for s in sorted(suites, key=lambda x: x["name"]):
        name = s["name"]
        status = (s.get("status") or "").lower()
        if status in ("error", "failed"):
            icon, build_cell, run_cell = "❌", "failed", "—"
        elif status == "filtered":
            icon, build_cell, run_cell = "⏭️", "n/a for this board", "—"
        else:
            build_cell = "ok"
            r = run_by_name.get(name)
            if r is None:
                icon, run_cell = "❌", "never ran"
            elif r["status"] == "pass":
                icon, run_cell = "✅", "ok"
            else:
                icon, run_cell = "❌", r["note"]
        lines.append(f"| {icon} | `{name}` | {build_cell} | {run_cell} |")
    lines.append("")

    if filtered:
        lines += [
            f"{len(filtered)} sample(s) do not apply to `{board}` (they need a "
            "display shield or a sensor) and were skipped by twister.",
            "",
        ]

    never_ran = [n for n in expected_to_run(suites) if n not in {r["name"] for r in runs}]
    if never_ran:
        lines += [
            f"{len(never_ran)} sample(s) built but produced no run result, so "
            "they were never executed. The run step reports one line per "
            "sample, so its log shows where it stopped:",
            "",
        ]
        lines += [f"- `{n}`" for n in never_ran]
        lines += [""]

    for s in failures:
        lines += [
            f"#### Build failure: `{s['name']}`",
            "",
            "```",
            (s.get("reason") or "no reason reported").strip()[:2000],
            "```",
            "",
            repro_block(s["name"], s.get("path", ""), board),
            "",
        ]

    suite_path = {s["name"]: s.get("path", "") for s in suites}
    for r in runs:
        if r["status"] == "pass":
            continue
        lines += [
            f"#### Run failure: `{r['name']}` — {r['note']}",
            "",
            "Last output before it stopped:",
            "",
            "```",
            (r.get("output") or "(no output)").strip()[-2000:],
            "```",
            "",
            repro_block(r["name"], suite_path.get(r["name"], ""), board),
            "",
        ]

    return "\n".join(lines)


def cmd_report(args) -> int:
    out = Path(args.output)

    if args.setup_error:
        # The workspace never got to a state where samples could be built, so
        # there are no per-sample results to report -- say that plainly instead
        # of letting an empty twister run look like "nothing was discovered".
        summary = "the Zephyr workspace could not be prepared"
        write_verdict(args.verdict, ICON_FAIL, summary)
        if write_pr_report(out, ICON_FAIL, summary, args.setup_error):
            print(f"wrote {out}")
        print(f"[{ICON_FAIL}] {summary}")
        return 0

    outdir = Path(args.outdir)
    suites = load_twister(outdir)
    runs = []
    if args.results and Path(args.results).is_file():
        runs = json.loads(Path(args.results).read_text()).get("samples", [])

    failures = build_failures(suites)
    run_failures = [r for r in runs if r["status"] != "pass"]
    filtered = [s for s in suites if (s.get("status") or "").lower() == "filtered"]
    # A sample that built but has no run result was never executed -- the run
    # step died, or died partway through. Not counting that as a failure would
    # let "built and ran clean" through for samples that never ran at all.
    ran = {r["name"] for r in runs}
    not_run = [name for name in expected_to_run(suites) if name not in ran]
    built_ok = len(suites) - len(failures) - len(filtered)

    remaps = source_remaps(outdir)
    patches = [p.strip() for p in (args.patches or "").split(",") if p.strip()]
    version_note = args.version_note.strip()
    outdated = version_note.lower().startswith("outdated")

    if failures or run_failures or not_run:
        icon = ICON_FAIL
        parts = []
        if failures:
            parts.append(f"{len(failures)} build failure(s)")
        if run_failures:
            parts.append(f"{len(run_failures)} run failure(s)")
        if not_run:
            parts.append(f"{len(not_run)} sample(s) never ran")
        summary = ", ".join(parts) + f" out of {len(suites) - len(filtered)} sample(s)"
    elif not suites:
        # Either twister never ran, or it found nothing. Both mean this check
        # verified nothing, so it must not report success.
        icon, summary = ICON_FAIL, "no samples were built - the check did not run"
    elif outdated:
        icon = ICON_WARN
        summary = (f"{built_ok} sample(s) built and ran clean, but a newer "
                   "Zephyr has been released")
    else:
        icon = ICON_OK
        summary = f"{built_ok} sample(s) built and ran clean on {args.board}"
        if remaps:
            summary += f" ({len(remaps)} Zephyr source path(s) reconciled)"

    details = render_details(suites, runs, args.board, args.pin, version_note,
                             patches, remaps)

    write_verdict(args.verdict, icon, summary)
    if write_pr_report(out, icon, summary, details):
        print(f"wrote {out}")
    print(f"[{icon}] {summary}")

    if args.details:
        Path(args.details).write_text(details)
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)

    r = sub.add_parser("run", help="run every built sample and record the outcome")
    r.add_argument("--outdir", required=True, help="twister output directory")
    r.add_argument("--results", required=True, help="where to write the run results JSON")
    r.add_argument("--seconds", type=int, default=20,
                   help="how long each sample is left running (default 20)")
    r.set_defaults(func=cmd_run)

    p = sub.add_parser("report", help="render the PR report JSON")
    p.add_argument("--outdir", default="", help="twister output directory")
    p.add_argument("--results", default="", help="run results JSON from 'run'")
    p.add_argument("--setup-error", default="",
                   help="report that the workspace could not be prepared, with "
                        "this markdown as the details")
    p.add_argument("--output", required=True, help="where to write the report JSON")
    p.add_argument("--details", default="", help="also write the details markdown here")
    p.add_argument("--verdict", default="",
                   help="write the pass/fail verdict here, independently of the "
                        "PR report")
    p.add_argument("--board", default="native_sim/native/64")
    p.add_argument("--pin", default="", help="the Zephyr revision that was used")
    p.add_argument("--version-note", default="", help="output of manifest.py check-version")
    p.add_argument("--patches", default="", help="comma separated list of applied patches")
    p.set_defaults(func=cmd_report)

    args = ap.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
