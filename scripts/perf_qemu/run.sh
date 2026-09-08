#!/bin/bash
# Run the QEMU instruction-count benchmark locally, in the same container CI uses, so the
# numbers match. Everything happens inside the container; nothing has to be installed here
# but Docker.
#
#   scripts/perf_qemu/run.sh                         # everything, on this working tree
#   scripts/perf_qemu/run.sh -t riscv32 -s render    # one target, one suite
#   scripts/perf_qemu/run.sh -f rgb888 --opt Os      # another format and -Os
#   scripts/perf_qemu/run.sh -t cortex-m7 -s render -f rgb565 --gdb
#
# -t, -f and -s take comma separated lists, so several of anything is one run:
#
#   scripts/perf_qemu/run.sh -f rgb565,rgb888              # two colour formats
#   scripts/perf_qemu/run.sh -t cortex-m7,riscv32 -f l8    # two targets
#
# A -s entry can name one scene, which is measured on its own instead of the whole suite.
# A render scene without an _opa_ suffix covers both opacities:
#
#   scripts/perf_qemu/run.sh -s render:fill                # both opacities of one scene
#   scripts/perf_qemu/run.sh -s render:fill_opa_128        # just that one
#   scripts/perf_qemu/run.sh -s bench:moving_wallpaper     # one benchmark scene
#   scripts/perf_qemu/run.sh -s bench:multiple_arcs,render:text
#
# --base takes either a directory of results-*.json from an earlier run, or a git revision
# to measure. A revision is measured in a worktree of its own, stored under <out>/base and
# reused on every later run with the same settings, so master is measured once:
#
#   scripts/perf_qemu/run.sh --base master           # measure master once, then reuse it
#   scripts/perf_qemu/run.sh --base ~/master-results # results saved from an earlier run
#
# Under --gdb the run stops before its first instruction and waits. Attach with
#   gdb-multiarch -ex 'target remote :1234' <out>/build/<target>-<opt>/<suite>_<cf>.elf
# The port is published from the container, so gdb runs on the host.
#
# To end a --gdb run press Ctrl-A then X. Ctrl-C and Ctrl-D do not work, because QEMU runs
# with -nographic and takes the terminal, with Ctrl-A as its escape key.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/../.." && pwd)"

# Pinned by digest, not by tag: republishing :1 would change every count without an LVGL
# change, and a stale local copy of the tag would disagree with a fresh pull in CI. Update
# this and perf_qemu.yml's `container:` together whenever the toolchain image changes.
IMAGE="${PERF_QEMU_IMAGE:-ghcr.io/lvgl/lv-perf-qemu@sha256:66cf814ae11c32e4eba451e95b30b24fa53b181c8f1f8bd47c2edc5ec59d2a77}"

LVGL="$REPO"
OUT="$REPO/build/perf_qemu"
TARGETS=""
FORMATS=""
SUITES=""
OPT=""
BASE_REF=""
JOBS=""
TOLERANCE=""
GDB_PORT=""
KEEP_DUMPS=""
BUILD_IMAGE=""
SHELL_ONLY=""

usage() {
    sed -n '2,/^set -euo/p' "$HERE/run.sh" | sed 's/^# \{0,1\}//;$d'
    cat <<'USAGE'
Options:
  -t, --targets LIST   cortex-m7, cortex-m55, riscv32, cortex-a53 (default all four)
  -f, --formats LIST   rgb565, rgb565_swapped, rgb888, xrgb8888, argb8888,
                       argb8888_premultiplied, l8, al88, i1
                       (default rgb565,argb8888, which is what CI measures)
  -s, --suites LIST    bench, render, either optionally narrowed to one scene as
                       <suite>:<scene> (default both suites, every scene)
      --opt LEVEL      Os, O2 or O3 (default O2, which is what CI measures)
  -b, --base REF|DIR   compare against a baseline and print the diff. A directory is read
                       as saved results-*.json; anything else is measured as a git
                       revision, stored under <out>/base and reused while that revision
                       and these settings are unchanged.
  -l, --lvgl DIR       the LVGL tree to measure (default: this repository)
  -o, --out DIR        where builds, results and error images go, on this machine
                       (default <repo>/build/perf_qemu). It is mounted at /out in the
                       container, which is the path CMake and QEMU print.
  -j, --jobs N         parallel QEMU runs (default: cores - 1)
      --tolerance N    per-channel difference against the reference images that still
                       counts as equal (default 0)
      --keep-dumps     keep the raw framebuffers, about 60 MB per format
  -g, --gdb [PORT]     stop and wait for gdb (default port 1234); needs a single run.
                       End the run with Ctrl-A then X, not Ctrl-C.
      --build-image    build the container image from the Dockerfile here
      --shell          open a shell in the container instead of running anything
  -h, --help

Every LIST is comma separated, so "-f rgb565,rgb888 -t cortex-m7,riscv32" measures two
formats on two targets in a single run.
USAGE
}

while [ $# -gt 0 ]; do
    case "$1" in
        -t|--targets) TARGETS="$2"; shift 2 ;;
        -f|--formats) FORMATS="$2"; shift 2 ;;
        -s|--suites) SUITES="$2"; shift 2 ;;
        --opt) OPT="$2"; shift 2 ;;
        -b|--base) BASE_REF="$2"; shift 2 ;;
        -l|--lvgl) LVGL="$(cd "$2" && pwd)"; shift 2 ;;
        -o|--out) OUT="$2"; shift 2 ;;
        -j|--jobs) JOBS="$2"; shift 2 ;;
        --tolerance) TOLERANCE="$2"; shift 2 ;;
        --keep-dumps) KEEP_DUMPS=1; shift ;;
        -g|--gdb)
            if [ "${2:-}" ] && [ -z "${2##[0-9]*}" ]; then GDB_PORT="$2"; shift 2
            else GDB_PORT=1234; shift; fi ;;
        --build-image) BUILD_IMAGE=1; shift ;;
        --shell) SHELL_ONLY=1; shift ;;
        -h|--help) usage; exit 0 ;;
        *) echo "unknown option: $1" >&2; usage >&2; exit 2 ;;
    esac
done

# Nothing but Docker is needed here: the toolchains and emulators come from the published
# image, which is the same one CI runs, so the counts match. Building it is the fallback,
# not the normal path.
if [ -n "$BUILD_IMAGE" ]; then
    IMAGE=lv-perf-qemu:local
    docker build -t "$IMAGE" "$HERE"
elif ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    echo "pulling $IMAGE (about 900 MB, once)"
    if ! docker pull "$IMAGE"; then
        # Said out loud rather than swallowed: if the package went private, or the tag was
        # never published, every run would otherwise quietly build its own 4 GB image and
        # nobody would know why the first run took ten minutes.
        echo >&2
        echo "could not pull $IMAGE, so building it from $HERE/Dockerfile instead." >&2
        echo "That works, but takes a few minutes and needs to download the toolchains." >&2
        echo "If it should have been pullable, check that the package is public." >&2
        IMAGE=lv-perf-qemu:local
        docker build -t "$IMAGE" "$HERE"
    fi
fi

mkdir -p "$OUT" 2>/dev/null || true
if [ ! -w "$OUT" ]; then
    echo "cannot write to $OUT." >&2
    echo "If a previous run left it owned by another user, remove it and try again, or" >&2
    echo "pick somewhere else with -o." >&2
    exit 1
fi
OUT="$(cd "$OUT" && pwd)"

# Everything below runs in the container, where this directory is /out, so the paths in
# CMake's and QEMU's output all start with /out. Say what that is out here, first thing,
# or "/out/build/..." reads like a directory in the filesystem root.
echo "output directory: $OUT  (/out inside the container)"

# The measured tree is mounted read-only so a benchmark run cannot dirty it. HOME points
# at the output directory because CMake and ccache want somewhere to write.
DOCKER_ARGS=(--rm -v "$REPO:/repo:ro" -v "$LVGL:/lvgl:ro" -v "$OUT:/out"
             -u "$(id -u):$(id -g)" -w /out -e HOME=/out)
[ -n "$GDB_PORT" ] && DOCKER_ARGS+=(-p "$GDB_PORT:$GDB_PORT")
[ -t 0 ] && DOCKER_ARGS+=(-it)

if [ -n "$SHELL_ONLY" ]; then
    exec docker run "${DOCKER_ARGS[@]}" "$IMAGE" bash
fi

# git works out here but usually not inside the container: this checkout can be a
# submodule, whose .git points outside the mount. Only a clean tree gets an identity;
# without one, --reuse will not stand a stored baseline in for a fresh measurement.
tree_id() {  # <dir>
    if [ -n "$(git -C "$1" status --porcelain 2>/dev/null)" ]; then
        echo ""
    else
        git -C "$1" rev-parse HEAD 2>/dev/null || echo ""
    fi
}

SELECT=()
[ -n "$TARGETS" ]   && SELECT+=(--targets "$TARGETS")
[ -n "$FORMATS" ]   && SELECT+=(--formats "$FORMATS")
[ -n "$SUITES" ]    && SELECT+=(--suites "$SUITES")
[ -n "$OPT" ]       && SELECT+=(--opt "$OPT")
[ -n "$JOBS" ]      && SELECT+=(--jobs "$JOBS")
[ -n "$TOLERANCE" ] && SELECT+=(--tolerance "$TOLERANCE")

RUN_ARGS=("${SELECT[@]}")
[ -n "$KEEP_DUMPS" ] && RUN_ARGS+=(--keep-dumps)
[ -n "$GDB_PORT" ]   && RUN_ARGS+=(--gdb "$GDB_PORT")

BASE_MOUNT=()
BASE_IN_CONTAINER=""
if [ -d "$BASE_REF" ]; then
    # Results saved from an earlier run, on this machine or someone else's
    BASE_IN_CONTAINER=/basedir
    BASE_MOUNT=(-v "$(cd "$BASE_REF" && pwd):/basedir:ro")
    echo "baseline: saved results in $BASE_REF"
elif [ -n "$BASE_REF" ]; then
    # A worktree rather than a checkout, so the working tree being measured is untouched.
    # It is kept between runs, and --reuse then skips the measurement itself as well.
    # From $LVGL, the tree being measured, not from $REPO where this script lives. With
    # -l pointing somewhere else the two are different repositories.
    BASE_TREE="$OUT/base-tree"
    if [ -d "$BASE_TREE" ]; then
        git -C "$BASE_TREE" checkout --detach "$BASE_REF"
    else
        git -C "$LVGL" worktree add --detach "$BASE_TREE" "$BASE_REF"
    fi
    echo "baseline: $BASE_REF ($(git -C "$BASE_TREE" rev-parse --short HEAD))"
    docker run "${DOCKER_ARGS[@]}" -v "$BASE_TREE:/base:ro" "$IMAGE" \
        python3 /repo/scripts/perf_qemu/perf_qemu.py \
        --lvgl /base --out /out/base --reuse \
        --tree-id "$(tree_id "$BASE_TREE")" "${SELECT[@]}"
    BASE_IN_CONTAINER=/out/base
fi

docker run "${DOCKER_ARGS[@]}" "$IMAGE" \
    python3 /repo/scripts/perf_qemu/perf_qemu.py --lvgl /lvgl --out /out \
    --tree-id "$(tree_id "$LVGL")" "${RUN_ARGS[@]}"

[ -n "$GDB_PORT" ] && exit 0

REPORT_ARGS=(--new /out --output /out/render_performance.json)
[ -n "$BASE_IN_CONTAINER" ] && REPORT_ARGS+=(--base "$BASE_IN_CONTAINER")
docker run "${DOCKER_ARGS[@]}" "${BASE_MOUNT[@]}" "$IMAGE" \
    python3 /repo/scripts/perf_qemu/report.py "${REPORT_ARGS[@]}"
docker run "${DOCKER_ARGS[@]}" "$IMAGE" python3 -c "
import json, pathlib
r = json.loads(pathlib.Path('/out/render_performance.json').read_text())
print()
print(r['icon'], r['section'] + ':', r['summary'])
print()
print(r['details'])
"

echo
echo "results and error images: $OUT"
