#!/usr/bin/env bash
#
# Build the LVGL test Docker image (linux/amd64) and run the test suite
# inside it. Works on non-x86_64 hosts (e.g. Apple Silicon) via QEMU.
#
# Package prerequisite checks from scripts/run_tests.sh are omitted: the
# container is built from tests/Dockerfile which guarantees they are present.
#
# Usage:
#   scripts/run_tests_docker.sh [options] [-- <extra args for tests/main.py>]
#
# Options:
#   --32                  Build/run as 32-bit (sets NON_AMD64_BUILD=1).
#   --64                  Build/run as 64-bit (default).
#   --both                Run both 64-bit and 32-bit, back to back.
#   --build-option <OPT>  Pass --build-options=<OPT> to tests/main.py
#                         (e.g. OPTIONS_TEST_SYSHEAP, OPTIONS_16BIT, ...).
#                         When omitted, main.py runs its full matrix.
#   --test-suite <REGEX>  Pass --test-suite=<REGEX> to tests/main.py
#                         (ctest --tests-regex filter).
#   --actions <LIST>      Space-separated list of actions for main.py
#                         (default: "build test"). E.g. --actions test.
#   --no-clean            Do not pass --clean to tests/main.py.
#   --report              Generate the coverage report (main.py --report)
#                         and run scripts/check_gcov_coverage.py afterwards.
#   --rebuild             Force rebuild of the Docker image (--no-cache).
#   --shell               Drop into an interactive bash shell in the container.
#   --                    Everything after -- is forwarded verbatim to
#                         tests/main.py.
#
# Environment:
#   LVGL_TEST_IMAGE       Override the image name (default: lvgl-tests:local).
#
# Examples:
#   # Full 64-bit build+test matrix with coverage
#   scripts/run_tests_docker.sh --report
#
#   # Single 32-bit test config, only run tests (no build-only matrix)
#   scripts/run_tests_docker.sh --32 --build-option OPTIONS_TEST_SYSHEAP \
#       --actions test
#
#   # Run only tests whose ctest name matches a regex
#   scripts/run_tests_docker.sh --test-suite 'test_obj.*'
#
#   # Interactive shell
#   scripts/run_tests_docker.sh --shell

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

IMAGE_NAME=${LVGL_TEST_IMAGE:-lvgl-tests:local}
DOCKERFILE="$REPO_ROOT/tests/Dockerfile"
PLATFORM="linux/amd64"

bits="64"
build_option=""
test_suite=""
actions="build test"
clean="--clean"
report=""
rebuild=false
shell=false
extra_args=()

usage() { sed -n '2,47p' "$0"; }

while [ "$#" -gt 0 ]; do
    case "$1" in
        --32)           bits="32" ;;
        --64)           bits="64" ;;
        --both)         bits="both" ;;
        --build-option) build_option="$2"; shift ;;
        --test-suite)   test_suite="$2"; shift ;;
        --actions)      actions="$2"; shift ;;
        --no-clean)     clean="" ;;
        --report)       report="--report" ;;
        --rebuild)      rebuild=true ;;
        --shell)        shell=true ;;
        --)             shift; extra_args=("$@"); break ;;
        -h|--help)      usage; exit 0 ;;
        *) echo "Unknown argument: $1" >&2; usage >&2; exit 1 ;;
    esac
    shift
done

if ! command -v docker >/dev/null 2>&1; then
    echo "Error: docker is not installed or not in PATH." >&2
    exit 1
fi

# --- Build image if needed -------------------------------------------------
build_args=(--platform "$PLATFORM" -f "$DOCKERFILE" -t "$IMAGE_NAME")
if [ "$rebuild" = true ]; then
    build_args+=(--no-cache)
fi

if [ "$rebuild" = true ] || ! docker image inspect "$IMAGE_NAME" >/dev/null 2>&1; then
    echo "Building Docker image '$IMAGE_NAME' (platform=$PLATFORM)..."
    docker build "${build_args[@]}" "$REPO_ROOT"
else
    echo "Using existing Docker image '$IMAGE_NAME' (use --rebuild to force rebuild)."
fi

# --- Docker run ------------------------------------------------------------
run_args=(
    --rm
    --platform "$PLATFORM"
    -v "$REPO_ROOT":/work
    -w /work
)

if [ -t 0 ] && [ -t 1 ]; then
    run_args+=(-it)
fi

if [ "$shell" = true ]; then
    exec docker run "${run_args[@]}" "$IMAGE_NAME" bash
fi

# --- Build the command to run inside the container -------------------------
# Assemble tests/main.py arguments.
main_args=()
[ -n "$clean" ]        && main_args+=("$clean")
[ -n "$report" ]       && main_args+=("$report")
[ -n "$build_option" ] && main_args+=("--build-options=$build_option")
[ -n "$test_suite" ]   && main_args+=("--test-suite=$test_suite")
# Actions go last, as positional arguments.
# shellcheck disable=SC2206
actions_arr=($actions)
main_args+=("${actions_arr[@]}")
main_args+=("${extra_args[@]}")

# Quote arguments safely for the inline bash script.
quote() { printf "%q " "$@"; }
main_args_q=$(quote "${main_args[@]}")

report_flag=$([ -n "$report" ] && echo 1 || echo 0)

# Inline script executed in the container. Mirrors relevant setup from
# scripts/run_tests.sh (gcov selection, NON_AMD64_BUILD, report renaming,
# coverage check) but skips the package-presence checks.
INNER=$(cat <<EOS
set -euo pipefail
cd /work

# Match gcov to the active gcc major version (gcovr honours \$GCOV).
gcc_major=\$(gcc -dumpversion | cut -d'.' -f1)
if command -v "gcov-\${gcc_major}" >/dev/null 2>&1; then
    export GCOV="gcov-\${gcc_major}"
else
    export GCOV="gcov"
fi
echo "Using GCOV=\$GCOV"

run_one() {
    local bits="\$1"
    if [ "\$bits" = "32" ]; then
        export NON_AMD64_BUILD=1
        echo "=== Running tests/main.py for 32-bit build ==="
    else
        unset NON_AMD64_BUILD
        echo "=== Running tests/main.py for 64-bit build ==="
    fi

    ./tests/main.py ${main_args_q}

    if [ "${report_flag}" = "1" ] && [ -d tests/report ]; then
        rm -rf "tests/report-\${bits}bit"
        mv tests/report "tests/report-\${bits}bit"
        echo "Coverage report: tests/report-\${bits}bit/index.html"
    fi
}

BITS="${bits}"
if [ "\$BITS" = "both" ]; then
    run_one 64
    run_one 32
else
    run_one "\$BITS"
fi

if [ "${report_flag}" = "1" ]; then
    echo "=== Running scripts/check_gcov_coverage.py ==="
    ./scripts/check_gcov_coverage.py
fi
EOS
)

exec docker run "${run_args[@]}" "$IMAGE_NAME" bash -lc "$INNER"
