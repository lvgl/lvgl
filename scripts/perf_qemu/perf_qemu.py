#!/usr/bin/env python3
"""
Build and run the QEMU instruction-count benchmark for one LVGL tree.

Meant to run inside the container from scripts/perf_qemu/Dockerfile; run.sh is the wrapper
that starts it. It builds the tree once per target and then fans the runs out, because QEMU
under -icount is deterministic and single threaded per process, so N runs at once give
byte-identical numbers to N in sequence.

The tree to measure is an argument rather than this file's own repository, so that the same
harness can measure a baseline worktree that does not contain scripts/perf_qemu.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent

TARGETS = {
    "cortex-m7": [
        "qemu-system-arm", "-M", "mps2-an500", "-cpu", "cortex-m7",
        "-nographic", "-semihosting", "-icount", "shift=0",
    ],
    "cortex-m55": [
        "qemu-system-arm", "-M", "mps3-an547", "-cpu", "cortex-m55",
        "-nographic", "-semihosting", "-icount", "shift=0",
    ],
    "riscv32": [
        "qemu-system-riscv32", "-M", "virt", "-m", "512M",
        "-nographic", "-semihosting", "-icount", "shift=0", "-bios", "none",
    ],
    "cortex-a53": [
        "qemu-system-aarch64", "-M", "virt", "-cpu", "cortex-a53", "-m", "512M",
        "-nographic", "-nodefaults", "-semihosting", "-icount", "shift=0",
    ],
}

# One per directory under tests/ref_imgs/draw/render, and one per PERF_QEMU_CF_* in
# harness/bench.h. CI runs a subset; the rest are here to be tried locally.
FORMATS = [
    "rgb565", "rgb565_swapped", "rgb888", "xrgb8888",
    "argb8888", "argb8888_premultiplied", "l8", "al88", "i1",
]
SUITES = ["bench", "render"]
OPTS = ["Os", "O2", "O3"]

# The selector that means "every scene", see run_image()
ALL_SCENES = "*"

# What CI measures, and the default here so that a bare run reproduces it
CI_FORMATS = ["rgb565", "argb8888"]
CI_OPT = "O2"

SC_RE = re.compile(r"^#SC\t(\S+)\t(\d+)$", re.M)
TOTAL_RE = re.compile(r"^#TOTAL\t(\d+)\t(\d+)$", re.M)
MEM_RE = re.compile(r"^#MEM\t(\d+)\t(\d+)$", re.M)
ERR_RE = re.compile(r"^#ERR\t(.*)$", re.M)

# A heap this full is close enough to thrashing the cache that the counts stop being a
# property of the code alone
MEM_WARN_PCT = 85


def build(lvgl: Path, target: str, opt: str, formats: list[str], build_dir: Path) -> None:
    """Configure and build every image for one target.

    Configured on every call, not only the first. The colour format list decides which
    images exist, so skipping this when the build directory is already there left a run
    with a different --formats looking for an executable nothing had built, and reported
    it as QEMU failing to load a kernel.
    """
    cmd = [
        "cmake", "-GNinja",
        "-S", str(HERE / "harness"), "-B", str(build_dir),
        f"-DCMAKE_TOOLCHAIN_FILE={HERE / 'cmake' / 'toolchain.cmake'}",
        f"-DPERF_QEMU_TARGET={target}",
        f"-DPERF_QEMU_OPT={opt}",
        "-DPERF_QEMU_FORMATS=" + ";".join(formats),
        f"-DPERF_QEMU_LVGL_DIR={lvgl}",
        "-DCMAKE_BUILD_TYPE=Release",
    ]
    # No ccache launcher here, though tests/main.py uses one and the image has ccache:
    # every compile carries --specs=, which ccache will not cache (measured: 591 of 591
    # calls uncacheable). Getting the libc include path onto the compile line another way
    # would fix it, see the note in the session's notes.md.
    subprocess.run(cmd, check=True)
    subprocess.run(["cmake", "--build", str(build_dir)], check=True)


def parse_suite(entry: str) -> tuple[str, str]:
    """Split a -s entry into its suite and the scene it selects, "" for all of them."""
    suite, _, scene = entry.partition(":")
    if suite not in SUITES:
        raise ValueError(f"unknown suite {suite!r}, expected one of {', '.join(SUITES)}")
    return suite, scene


def run_image(target: str, elf: Path, cwd: Path, timeout: int, gdb_port: int | None,
              scene: str = "") -> str:
    # The scene is passed at run time rather than compiled in, so selecting one does not
    # mean linking an executable per scene; the target reads it with semihosting's
    # SYS_GET_CMDLINE. Always passed, including the "*" that means every scene, because
    # with no arg at all QEMU answers that call with the kernel's path instead.
    cmd = list(TARGETS[target]) + [
        "-semihosting-config", f"enable=on,arg={scene or ALL_SCENES}",
        "-kernel", str(elf),
    ]
    if gdb_port is not None:
        # -S stops before the first instruction, so a breakpoint can be set on main
        cmd += ["-gdb", f"tcp::{gdb_port}", "-S"]
        # -nographic hands QEMU the terminal and its own escape sequences, so Ctrl-C goes
        # to the emulated machine rather than ending the run. Say how to get out, because
        # nothing else on screen does.
        print(
            f"waiting for gdb on port {gdb_port}. In another shell:\n"
            f"  gdb-multiarch -ex 'target remote :{gdb_port}' "
            f"<output directory>/{elf.relative_to(elf.parents[2])}\n"
            f"\n"
            f"To end this run press Ctrl-A then X. Ctrl-C and Ctrl-D do not work: QEMU\n"
            f"owns the terminal and Ctrl-A is its escape key (Ctrl-A then H lists the\n"
            f"rest). Killing the container from another shell also works.",
            flush=True)
        # Not check=True: a semihosting exit leaves QEMU's status at 1, so a run that
        # finished normally would raise. QEMU's own errors are on the terminal already.
        subprocess.run(cmd, cwd=cwd)
        return ""
    # QEMU writes semihosting output to stderr, and a semihosting exit leaves its status
    # at 1, so both streams are collected and the return code is not the check. parse() is.
    done = subprocess.run(cmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True, timeout=timeout)
    return done.stdout


def parse(label: str, out: str) -> dict:
    scenes = {name: int(value) for name, value in SC_RE.findall(out)}
    total = TOTAL_RE.search(out)
    if not scenes or not total:
        raise RuntimeError(f"{label}: no #SC or #TOTAL in the run output:\n" + out[-2000:])

    # The target reports its own problems this way. A framebuffer it could not write would
    # otherwise just be missing from the reference comparison, which would pass.
    errors = ERR_RE.findall(out)
    if errors:
        raise RuntimeError(f"{label}: the run reported " + "; ".join(errors))
    data = {"scenes": scenes, "total": int(total.group(1)), "frames": int(total.group(2))}

    mem = MEM_RE.search(out)
    if mem:
        used, size = int(mem.group(1)), int(mem.group(2))
        data["heap"] = {"max_used": used, "total": size}
        # With the assertions off a heap that ran out does not fail, it draws the wrong
        # thing, and one that merely got tight makes the cache evict and the counts move
        if size and used * 100 // size >= MEM_WARN_PCT:
            raise RuntimeError(
                f"{label}: LVGL's heap peaked at {used:,} of {size:,} bytes "
                f"({used * 100 // size}%). Raise CONFIG_LV_MEM_SIZE in "
                f"configs/perf_qemu.defconfig; this close to the limit the cache starts "
                f"evicting and the counts stop being a property of the code.")
    return data


def is_current(results: Path, settings: dict) -> bool:
    """Can a stored results file stand in for a run with these settings?

    Only when the tree had an identity. A dirty or unidentifiable tree gets an empty one,
    so it is always measured again.
    """
    if not settings["tree"] or not results.is_file():
        return False
    try:
        return json.loads(results.read_text()).get("settings") == settings
    except (OSError, json.JSONDecodeError):
        return False


def tree_id(lvgl: Path) -> str:
    """The revision measured, or "" if it has no stable identity.

    Empty for a modified tree, and for one git cannot read: a submodule checkout's .git
    points outside the container, which is the usual case for a local run. run.sh works
    that out on the host and passes it in with --tree-id instead.
    """
    def git(*args) -> str | None:
        try:
            return subprocess.run(["git", "-c", "safe.directory=*", "--no-optional-locks",
                                   "-C", str(lvgl), *args],
                                  capture_output=True, text=True, check=True).stdout.strip()
        except (subprocess.CalledProcessError, OSError):
            return None

    head = git("rev-parse", "HEAD")
    status = git("status", "--porcelain")
    if head is None or status is None or status:
        return ""
    return head


def compare(lvgl: Path, dumps: Path, cf: str, out_json: Path, err_dir: Path,
            tolerance: int) -> dict:
    refs = lvgl / "tests" / "ref_imgs" / "draw" / "render" / cf
    cmd = [sys.executable, str(HERE / "compare_refs.py"),
           "--dumps", str(dumps), "--cf", cf, "--output", str(out_json),
           "--err-dir", str(err_dir), "--tolerance", str(tolerance)]
    if refs.is_dir():
        cmd += ["--refs", str(refs)]
    subprocess.run(cmd, check=True)
    return json.loads(out_json.read_text())


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--lvgl", required=True, type=Path, help="the LVGL tree to measure")
    ap.add_argument("--out", required=True, type=Path, help="where results and builds go")
    ap.add_argument("--targets", default=",".join(TARGETS),
                    help=f"any of {', '.join(TARGETS)}")
    ap.add_argument("--formats", default=",".join(CI_FORMATS),
                    help=f"any of {', '.join(FORMATS)} (default: what CI measures)")
    ap.add_argument("--suites", default=",".join(SUITES),
                    help="bench and/or render, each optionally narrowed to one scene with "
                         "'<suite>:<scene>'. A render scene without an _opa_ suffix takes "
                         "both opacities. For example "
                         "'render:fill', 'render:fill_opa_128', 'bench:moving_wallpaper'.")
    ap.add_argument("--opt", default=CI_OPT, choices=OPTS,
                    help="optimisation level (default: what CI measures)")
    ap.add_argument("--tree-id", default=None,
                    help="the revision being measured, when git cannot be asked inside the "
                         "container. Empty means the tree has no stable identity and "
                         "--reuse will not reuse a stored result for it.")
    ap.add_argument("--reuse", action="store_true",
                    help="skip a target whose results file already records this exact "
                         "revision and settings. For a stored baseline, so that a local "
                         "run does not measure master again.")
    ap.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 2) - 1))
    ap.add_argument("--timeout", type=int, default=3600, help="seconds per QEMU run")
    ap.add_argument("--tolerance", type=int, default=0,
                    help="per-channel difference against the reference images that still "
                         "counts as equal")
    ap.add_argument("--keep-dumps", action="store_true",
                    help="keep the raw framebuffers; they are ~60 MB per format")
    ap.add_argument("--gdb", type=int, metavar="PORT",
                    help="stop the run and wait for gdb. Only with a single run selected.")
    args = ap.parse_args()

    lvgl = args.lvgl.resolve()
    if not (lvgl / "Kconfig").is_file():
        print(f"{lvgl} does not look like an LVGL tree", file=sys.stderr)
        return 1

    targets = args.targets.split(",")
    formats = args.formats.split(",")
    try:
        # [(suite, scene)], where an empty scene means the whole suite
        selection = [parse_suite(e) for e in args.suites.split(",")]
    except ValueError as err:
        print(err, file=sys.stderr)
        return 1
    suites = sorted({suite for suite, _ in selection})
    unknown = [t for t in targets if t not in TARGETS]
    if unknown:
        print(f"unknown target(s): {', '.join(unknown)}", file=sys.stderr)
        return 1
    unknown = [f for f in formats if f not in FORMATS]
    if unknown:
        print(f"unknown colour format(s): {', '.join(unknown)}", file=sys.stderr)
        return 1

    # Everything that changes a number, so that --reuse can tell whether a stored result
    # still answers the question being asked
    settings = {"opt": args.opt, "formats": formats, "suites": args.suites.split(","),
                "tolerance": args.tolerance,
                "tree": tree_id(lvgl) if args.tree_id is None else args.tree_id}

    args.out.mkdir(parents=True, exist_ok=True)

    # A narrowed run in a directory a wider one already used would otherwise leave the
    # older targets' files behind, and report.py reads every results-*.json it finds.
    # Compared against what was asked for, not what is left after --reuse filters it.
    requested = set(targets)
    for stale in args.out.glob("results-*.json"):
        if stale.name[len("results-"):-len(".json")] not in requested:
            print(f"removing {stale.name}, not part of this run")
            stale.unlink()

    if args.reuse:
        fresh = [t for t in targets if not is_current(args.out / f"results-{t}.json", settings)]
        for target in sorted(set(targets) - set(fresh)):
            print(f"{target}: reusing {args.out / f'results-{target}.json'}")
        if not fresh:
            return 0
        targets = fresh

    # Deduplicated: the same (target, suite, format) twice would be two runs writing the
    # same framebuffer dumps at the same time.
    runs = list(dict.fromkeys(
        (t, suite, f, scene)
        for t in targets for suite, scene in selection for f in formats))
    if args.gdb is not None and len(runs) != 1:
        print(f"--gdb needs exactly one run selected, not {len(runs)}. Narrow it down with "
              f"--targets, --suites and --formats.", file=sys.stderr)
        return 1

    for target in targets:
        build(lvgl, target, args.opt, formats, args.out / "build" / f"{target}-{args.opt}")

    def do_run(job, gdb=None):
        target, suite, cf, scene = job
        elf = args.out / "build" / f"{target}-{args.opt}" / f"{suite}_{cf}.elf"
        cwd = args.out / "dumps" / target
        (cwd / cf).mkdir(parents=True, exist_ok=True)
        out = run_image(target, elf, cwd, args.timeout, gdb, scene)
        label = f"{target} {suite}{':' + scene if scene else ''} {cf}"
        return job, (None if gdb is not None else parse(label, out))

    if args.gdb is not None:
        do_run(runs[0], args.gdb)
        return 0

    results = {t: {"target": t, "settings": settings, "suites": {}} for t in targets}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for job, data in pool.map(do_run, runs):
            target, suite, cf, scene = job
            key = f"{suite}_{cf}"
            # Two scenes of the same suite share a key, so merge rather than overwrite
            have = results[target]["suites"].get(key)
            if have:
                have["scenes"].update(data["scenes"])
                have["total"] += data["total"]
                have["frames"] += data["frames"]
                if "heap" in data:
                    have["heap"] = data["heap"]
            else:
                results[target]["suites"][key] = data
            shown = f"{suite}:{scene}" if scene else suite
            print(f"{target} {shown} {cf}: {data['total']} instructions", flush=True)

    for target in targets:
        for cf in formats:
            if "render" not in suites:
                continue
            dumps = args.out / "dumps" / target / cf
            report = compare(lvgl, dumps, cf,
                             args.out / f"refcmp-{target}-{cf}.json",
                             args.out / "err_imgs" / target / cf,
                             args.tolerance)
            results[target]["suites"][f"render_{cf}"]["refs"] = report
            if not args.keep_dumps:
                shutil.rmtree(dumps)

        out = args.out / f"results-{target}.json"
        out.write_text(json.dumps(results[target], indent=1) + "\n")
        print(f"wrote {out}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
