#!/usr/bin/env python3
"""
Incremental cppcheck static analysis for LVGL CI.

Modes:
  --diff <range>   Check only files changed in a git diff (CI mode)
  --file <path>    Check a single file or directory
  --all            Full scan of all src/**/*.c src/**/*.h (audit mode)
  --self-test      Run built-in sanity checks

Exclusion: only src/libs/ (third-party code) is excluded.
"""

import argparse
import os
import subprocess
import sys
import time
from pathlib import Path
from typing import List, Dict

# ---------------------------------------------------------------------------
# Exclusion list
# ---------------------------------------------------------------------------
EXCLUDE_DIRS = [
    # Third-party library wrappers — not LVGL's own code
    "src/libs/",
]

# Severity levels that block merge
BLOCKING_SEVERITIES = {"error"}

# Severity levels that produce annotations but don't block
ANNOTATION_SEVERITIES = {"warning"}

CPPCHECK_LIBRARY = str(Path(__file__).resolve().parent / "lvgl_cppcheck.cfg")

CPPCHECK_COMMON_ARGS = [
    "--enable=all",
    "--quiet",
    "--inline-suppr",
    f"--library={CPPCHECK_LIBRARY}",
    "--suppress=unusedFunction",
    "--suppress=preprocessorErrorDirective",
    "--suppress=missingIncludeSystem",
    "--suppress=missingInclude",
    "--suppress=ConfigurationNotChecked",
    "--suppress=toomanyconfigs",
]

TEMPLATE = "{severity}|{file}|{line}|{id}|{message}"


def find_repo_root() -> Path:
    """Walk up from this script to find the repo root (contains src/)."""
    p = Path(__file__).resolve().parent.parent
    if (p / "src").is_dir():
        return p
    # fallback: cwd
    return Path.cwd()


def is_excluded(filepath: str) -> bool:
    """Check if a file path matches any exclusion pattern."""
    rel = filepath.replace("\\", "/")
    for exc in EXCLUDE_DIRS:
        if rel.startswith(exc) or f"/{exc}" in rel:
            return True
    return False


def _cppcheck_available() -> bool:
    """Check if cppcheck is installed."""
    try:
        r = subprocess.run(["cppcheck", "--version"], capture_output=True, text=True)
        return r.returncode == 0
    except FileNotFoundError:
        return False


def run_cppcheck(files: List[str], jobs: int = 1) -> List[Dict]:
    """Run cppcheck on a list of files and parse results."""
    if not files:
        return []

    cmd = (
        ["cppcheck"]
        + CPPCHECK_COMMON_ARGS
        + [
            f"--template={TEMPLATE}",
        ]
    )
    if jobs > 1:
        cmd.append(f"-j{jobs}")
    cmd.extend(files)

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
    except subprocess.TimeoutExpired as exc:
        print("cppcheck timed out after 600s", file=sys.stderr)
        raise RuntimeError("cppcheck timed out after 600s") from exc

    # cppcheck returns 0 even when findings exist.
    # Non-zero typically means internal error (bad args, crash).
    # Parse stderr regardless, but warn on unexpected exit codes.
    if result.returncode != 0:
        print(
            f"warning: cppcheck exited with code {result.returncode}", file=sys.stderr
        )

    if "Failed to load library" in result.stderr:
        raise RuntimeError(
            f"cppcheck failed to load library configuration:\n{result.stderr.strip()}"
        )

    issues = []
    for line in result.stderr.splitlines():
        parts = line.split("|", 4)
        if len(parts) == 5:
            severity, filepath, lineno, checker_id, message = parts
            if severity in ("error", "warning", "style", "performance", "portability"):
                issues.append(
                    {
                        "severity": severity,
                        "file": filepath,
                        "line": int(lineno) if lineno.isdigit() else 0,
                        "id": checker_id,
                        "message": message,
                    }
                )

    # If cppcheck failed AND produced no parseable output, that's a real problem
    if result.returncode != 0 and not issues and not result.stderr.strip():
        raise RuntimeError(
            f"cppcheck exited with code {result.returncode} and no output"
        )

    return issues


def get_changed_files(diff_range: str, root: Path) -> List[str]:
    """Get .c/.h files changed in a git diff range."""
    cmd = ["git", "diff", "--name-only", "--diff-filter=ACMR", diff_range]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=str(root))
    if result.returncode != 0:
        stderr = result.stderr.strip() or "unknown error"
        raise RuntimeError(
            f"git diff failed for range '{diff_range}' in '{root}': {stderr}"
        )

    files = []
    for f in result.stdout.strip().splitlines():
        f = f.strip()
        if (
            f
            and f.startswith("src/")
            and (f.endswith(".c") or f.endswith(".h"))
            and not is_excluded(f)
        ):
            full = root / f
            if full.exists():
                files.append(str(full))
    return files


def get_all_files(root: Path) -> List[str]:
    """Get all .c/.h files under src/, excluding paths in EXCLUDE_DIRS."""
    src_dir = root / "src"
    files = []
    for ext in ("*.c", "*.h"):
        for f in src_dir.rglob(ext):
            rel = str(f.relative_to(root))
            if not is_excluded(rel):
                files.append(str(f))
    return sorted(files)


def format_github_annotation(issue: Dict, root: Path) -> str:
    """Format an issue as a GitHub Actions annotation."""
    try:
        rel = os.path.relpath(issue["file"], str(root))
    except ValueError:
        rel = issue["file"]

    level = "error" if issue["severity"] in BLOCKING_SEVERITIES else "warning"

    line = issue.get("line", 0)
    attrs = [f"file={rel}"]
    if line > 0:
        attrs.append(f"line={line}")

    return f"::{level} {','.join(attrs)}::[cppcheck:{issue['id']}] {issue['message']}"


def print_summary(issues: List[Dict], root: Path) -> int:
    """Print results summary. Returns exit code (1 if blocking issues found)."""
    if not issues:
        print("✅ cppcheck: no issues found")
        return 0

    by_severity: Dict[str, List[Dict]] = {}
    for issue in issues:
        by_severity.setdefault(issue["severity"], []).append(issue)

    print(f"\ncppcheck found {len(issues)} issue(s):\n")

    for sev in ("error", "warning", "style", "performance", "portability"):
        group = by_severity.get(sev, [])
        if group:
            icon = (
                "🚫"
                if sev in BLOCKING_SEVERITIES
                else "⚠️" if sev in ANNOTATION_SEVERITIES else "📝"
            )
            print(f"  {icon} {sev}: {len(group)}")

    print()

    for sev in ("error", "warning"):
        group = by_severity.get(sev, [])
        for issue in group:
            try:
                rel = os.path.relpath(issue["file"], str(root))
            except ValueError:
                rel = issue["file"]
            print(
                f"  {rel}:{issue['line']}: {issue['severity']}: {issue['message']} [{issue['id']}]"
            )

    ci = os.environ.get("CI") or os.environ.get("GITHUB_ACTIONS")
    if ci:
        for issue in issues:
            if issue["severity"] in BLOCKING_SEVERITIES | ANNOTATION_SEVERITIES:
                print(format_github_annotation(issue, root))

    errors = by_severity.get("error", [])
    if errors:
        print(f"\n🚫 {len(errors)} error(s) found — these must be fixed before merge.")
        return 1

    return 0


def run_self_test() -> int:
    """Comprehensive sanity checks including cppcheck detection verification."""
    import tempfile

    print("Running self-tests...")
    passed = 0
    failed = 0
    cppcheck_ok = False

    def check(condition: bool, name: str):
        nonlocal passed, failed
        if condition:
            passed += 1
        else:
            failed += 1
            print(f"  ❌ FAIL: {name}")

    # --- 1. Exclusion logic ---
    check(is_excluded("src/libs/lodepng/lodepng.c"), "exclude src/libs/")
    check(is_excluded("src/libs/freetype/lv_freetype.c"), "exclude src/libs/ subdir")
    check(not is_excluded("src/core/lv_obj.c"), "include core")
    check(not is_excluded("src/widgets/chart/lv_chart.c"), "include widgets")
    check(not is_excluded("src/draw/nanovg/lv_nanovg.c"), "include draw drivers")
    check(
        not is_excluded("src/drivers/wayland/lv_wayland.c"), "include platform drivers"
    )
    check(
        not is_excluded("src/core/lv_libs_helper.c"), "include file with 'libs' in name"
    )
    print(f"  exclusion logic: {passed} passed")

    # --- 2. cppcheck availability ---
    cppcheck_ok = _cppcheck_available()
    check(cppcheck_ok, "cppcheck available")
    if cppcheck_ok:
        r = subprocess.run(["cppcheck", "--version"], capture_output=True, text=True)
        print(f"  cppcheck available: {r.stdout.strip()}")
    else:
        print("  ⚠️  cppcheck not found — skipping detection tests")

    # --- 3. Template parsing ---
    test_line = "error|test.c|42|nullPointer|Null pointer dereference"
    parts = test_line.split("|", 4)
    check(len(parts) == 5, "template split count")
    check(parts[0] == "error", "template severity")
    check(parts[2] == "42", "template line number")
    print(f"  template parsing: OK")

    # --- 4. Detection tests (only if cppcheck is available) ---
    if cppcheck_ok:
        print("  detection tests:")
        with tempfile.TemporaryDirectory() as tmpdir:
            test_cases = [
                {
                    "name": "null pointer dereference",
                    "code": "void f(void) { int *p = 0; *p = 1; }\n",
                    "expect_severity": "error",
                },
                {
                    "name": "uninitialized variable",
                    "code": "void f(void) { int x; int y = x + 1; (void)y; }\n",
                    "expect_severity": "error",
                },
                {
                    "name": "array out of bounds",
                    "code": "void f(void) { int a[10]; a[10] = 0; }\n",
                    "expect_severity": "error",
                },
                {
                    "name": "memory leak",
                    "code": "void* malloc(unsigned long);\nvoid f(void) { void *p = malloc(10); if(!p) return; }\n",
                    "expect_severity": "error",
                },
                {
                    "name": "variable scope (style)",
                    "code": "void f(int n) { int i; if(n > 0) { for(i = 0; i < n; i++) {} } }\n",
                    "expect_severity": "style",
                },
            ]

            for tc in test_cases:
                test_file = os.path.join(tmpdir, "test.c")
                with open(test_file, "w") as fh:
                    fh.write(tc["code"])

                issues = run_cppcheck([test_file], jobs=1)
                found = any(i["severity"] == tc["expect_severity"] for i in issues)
                check(found, f"detect {tc['name']}")
                status = "✅" if found else "❌"
                print(f"    {status} {tc['name']}")

            # --- 4b. Assert-aware suppression tests ---
            # Use LV_ASSERT_NULL (mapped by lvgl_cppcheck.cfg) to verify
            # the library config is actually loaded and effective.
            print("  assert suppression tests:")
            assert_test_cases = [
                {
                    "name": "LV_ASSERT_NULL suppresses nullPointerRedundantCheck",
                    "code": (
                        "void f(int *p) {\n"
                        "    LV_ASSERT_NULL(p);\n"
                        "    *p = 42;\n"
                        "}\n"
                    ),
                    "expect_no_ids": ["nullPointerRedundantCheck"],
                },
                {
                    "name": "LV_ASSERT_NULL on p does not suppress null deref on q",
                    "code": (
                        "void f(int *p, int *q) {\n"
                        "    LV_ASSERT_NULL(p);\n"
                        "    if(q) {}\n"
                        "    *q = 42;\n"
                        "}\n"
                    ),
                    "expect_ids": ["nullPointerRedundantCheck"],
                },
                {
                    "name": "without assert null deref is still detected",
                    "code": (
                        "void f(int *p) {\n" "    if(p) {}\n" "    *p = 42;\n" "}\n"
                    ),
                    "expect_ids": ["nullPointerRedundantCheck"],
                },
            ]

            for tc in assert_test_cases:
                test_file = os.path.join(tmpdir, "test.c")
                with open(test_file, "w") as fh:
                    fh.write(tc["code"])

                issues = run_cppcheck([test_file], jobs=1)
                issue_ids = {i["id"] for i in issues}
                if "expect_no_ids" in tc:
                    bad = issue_ids & set(tc["expect_no_ids"])
                    ok = len(bad) == 0
                else:
                    ok = all(eid in issue_ids for eid in tc["expect_ids"])
                check(ok, tc["name"])
                status = "✅" if ok else "❌"
                print(f"    {status} {tc['name']}")
    else:
        print("  detection tests: SKIPPED (cppcheck not found)")

    # --- 5. Annotation formatting ---
    root = Path("/fake/root")
    test_issue = {
        "severity": "error",
        "file": "/fake/root/src/core/lv_obj.c",
        "line": 42,
        "id": "nullPointer",
        "message": "test",
    }
    ann = format_github_annotation(test_issue, root)
    check("::error" in ann, "annotation level")
    check("line=42" in ann, "annotation line")
    check("cppcheck:nullPointer" in ann, "annotation checker id")

    test_issue_noline = {
        "severity": "warning",
        "file": "/fake/root/src/core/lv_obj.c",
        "line": 0,
        "id": "test",
        "message": "test",
    }
    ann2 = format_github_annotation(test_issue_noline, root)
    check("line=" not in ann2, "annotation omits line=0")
    print(f"  annotation formatting: OK")

    # --- Summary ---
    print(f"\nSelf-test: {passed} passed, {failed} failed")
    return 1 if failed else 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description="LVGL cppcheck static analysis checker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--diff",
        metavar="RANGE",
        help="Check files changed in git diff range (e.g. HEAD~3...HEAD)",
    )
    parser.add_argument(
        "--file", metavar="PATH", help="Check a specific file or directory"
    )
    parser.add_argument(
        "--all", action="store_true", help="Full scan of all src/**/*.c and src/**/*.h"
    )
    parser.add_argument(
        "--self-test", action="store_true", help="Run built-in sanity checks"
    )
    parser.add_argument(
        "--jobs",
        "-j",
        type=int,
        default=os.cpu_count() or 4,
        help="Number of parallel jobs (default: CPU count)",
    )
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")

    args = parser.parse_args()
    root = find_repo_root()

    if args.self_test:
        return run_self_test()

    if args.diff:
        print(f"Checking files changed in {args.diff}...")
        files = get_changed_files(args.diff, root)
        if not files:
            print("No .c/.h files changed (after exclusions). Nothing to check.")
            return 0
        print(f"  {len(files)} file(s) to check (excluded: src/libs/)")
    elif args.file:
        target = Path(args.file)
        if target.is_dir():
            files = [
                str(f)
                for f in sorted(target.rglob("*.c")) + sorted(target.rglob("*.h"))
                if not is_excluded(str(f))
            ]
        else:
            # Apply exclusion even for single file (Copilot fix)
            if is_excluded(str(target)):
                print(f"Skipped: {target} is in excluded path.")
                return 0
            files = [str(target)]
        if not files:
            print("No files to check.")
            return 0
        print(f"  {len(files)} file(s) to check")
    elif args.all:
        print("Full scan of src/ (excluding src/libs/)...")
        files = get_all_files(root)
        print(f"  {len(files)} file(s) to check")
    else:
        parser.print_help()
        return 1

    if args.verbose:
        for f in files:
            print(f"    {f}")

    start = time.time()
    issues = run_cppcheck(files, jobs=args.jobs)
    elapsed = time.time() - start

    print(f"  Scan completed in {elapsed:.1f}s ({len(files)} files, {args.jobs} jobs)")

    return print_summary(issues, root)


if __name__ == "__main__":
    sys.exit(main())
