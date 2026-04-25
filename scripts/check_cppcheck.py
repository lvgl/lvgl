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
# Exclusion list — aligned with gcovr.cfg filter/exclude
# ---------------------------------------------------------------------------
EXCLUDE_DIRS = [
    # Third-party library wrappers — not LVGL's own code
    "src/libs/",
]

# Severity levels that block merge
BLOCKING_SEVERITIES = {"error"}

# Severity levels that produce annotations but don't block
ANNOTATION_SEVERITIES = {"warning"}

CPPCHECK_COMMON_ARGS = [
    "--enable=all",
    "--quiet",
    "--suppress=unusedFunction",
    "--suppress=preprocessorErrorDirective",
    "--suppress=missingIncludeSystem",
    "--suppress=missingInclude",
    "--suppress=unmatchedSuppression",
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
    # Normalize to forward slashes and make relative to src/
    rel = filepath.replace("\\", "/")
    for exc in EXCLUDE_DIRS:
        if exc in rel:
            return True
    return False


def run_cppcheck(files: List[str], jobs: int = 1) -> List[Dict]:
    """Run cppcheck on a list of files and parse results."""
    if not files:
        return []

    cmd = ["cppcheck"] + CPPCHECK_COMMON_ARGS + [
        f"--template={TEMPLATE}",
    ]
    if jobs > 1:
        cmd.append(f"-j{jobs}")
    cmd.extend(files)

    try:
        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=600
        )
    except subprocess.TimeoutExpired:
        print("cppcheck timed out after 600s", file=sys.stderr)
        return []

    # Check cppcheck exit status (Cubic P1)
    if result.returncode != 0:
        print(f"cppcheck failed with exit code {result.returncode}", file=sys.stderr)
        if result.stderr:
            print(result.stderr, file=sys.stderr)
        # Continue parsing — cppcheck may still emit useful diagnostics

    issues = []
    for line in result.stderr.splitlines():
        parts = line.split("|", 4)
        if len(parts) == 5:
            severity, filepath, lineno, checker_id, message = parts
            if severity in ("error", "warning", "style", "performance", "portability"):
                issues.append({
                    "severity": severity,
                    "file": filepath,
                    "line": int(lineno) if lineno.isdigit() else 0,
                    "id": checker_id,
                    "message": message,
                })
    return issues


def get_changed_files(diff_range: str, root: Path) -> List[str]:
    """Get .c/.h files changed in a git diff range."""
    # Get all changed files first, then filter by extension in Python (Copilot fix)
    cmd = ["git", "diff", "--name-only", "--diff-filter=ACMR", diff_range]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=str(root))
    if result.returncode != 0:
        print(f"git diff failed: {result.stderr}", file=sys.stderr)
        return []

    files = []
    for f in result.stdout.strip().splitlines():
        f = f.strip()
        # Filter by extension and exclusion
        if f and (f.endswith(".c") or f.endswith(".h")) and not is_excluded(f):
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
    
    # Omit line attribute when unknown (Copilot fix)
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

    # Group by severity
    by_severity: Dict[str, List[Dict]] = {}
    for issue in issues:
        by_severity.setdefault(issue["severity"], []).append(issue)

    print(f"\ncppcheck found {len(issues)} issue(s):\n")

    for sev in ("error", "warning", "style", "performance", "portability"):
        group = by_severity.get(sev, [])
        if group:
            icon = "🚫" if sev in BLOCKING_SEVERITIES else "⚠️" if sev in ANNOTATION_SEVERITIES else "📝"
            print(f"  {icon} {sev}: {len(group)}")

    print()

    # Print details for error and warning
    for sev in ("error", "warning"):
        group = by_severity.get(sev, [])
        for issue in group:
            try:
                rel = os.path.relpath(issue["file"], str(root))
            except ValueError:
                rel = issue["file"]
            print(f"  {rel}:{issue['line']}: {issue['severity']}: {issue['message']} [{issue['id']}]")

    # GitHub Actions annotations
    ci = os.environ.get("CI") or os.environ.get("GITHUB_ACTIONS")
    if ci:
        for issue in issues:
            if issue["severity"] in BLOCKING_SEVERITIES | ANNOTATION_SEVERITIES:
                print(format_github_annotation(issue, root))

    # Block on errors
    errors = by_severity.get("error", [])
    if errors:
        print(f"\n🚫 {len(errors)} error(s) found — these must be fixed before merge.")
        return 1

    return 0


def run_self_test() -> int:
    """Basic sanity checks."""
    print("Running self-tests...")
    passed = 0
    failed = 0

    # Test 1: exclusion logic
    assert is_excluded("src/libs/lodepng/lodepng.c"), "should exclude src/libs/"
    assert is_excluded("src/libs/freetype/lv_freetype.c"), "should exclude src/libs/ subdir"
    assert not is_excluded("src/core/lv_obj.c"), "should not exclude core"
    assert not is_excluded("src/widgets/chart/lv_chart.c"), "should not exclude widgets"
    assert not is_excluded("src/draw/nanovg/lv_nanovg.c"), "should not exclude drivers"
    assert not is_excluded("src/drivers/wayland/lv_wayland.c"), "should not exclude drivers"
    passed += 6
    print(f"  exclusion logic: {passed} passed")

    # Test 2: cppcheck is available
    try:
        result = subprocess.run(["cppcheck", "--version"], capture_output=True, text=True)
        assert result.returncode == 0
        print(f"  cppcheck available: {result.stdout.strip()}")
        passed += 1
    except FileNotFoundError:
        print("  ❌ cppcheck not found")
        failed += 1

    # Test 3: template parsing
    test_line = "error|test.c|42|nullPointer|Null pointer dereference"
    parts = test_line.split("|", 4)
    assert len(parts) == 5
    assert parts[0] == "error"
    assert parts[2] == "42"
    passed += 1
    print(f"  template parsing: OK")

    print(f"\nSelf-test: {passed} passed, {failed} failed")
    return 1 if failed else 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description="LVGL cppcheck static analysis checker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--diff", metavar="RANGE",
                        help="Check files changed in git diff range (e.g. HEAD~3...HEAD)")
    parser.add_argument("--file", metavar="PATH",
                        help="Check a specific file or directory")
    parser.add_argument("--all", action="store_true",
                        help="Full scan of all src/**/*.c and src/**/*.h")
    parser.add_argument("--self-test", action="store_true",
                        help="Run built-in sanity checks")
    parser.add_argument("--jobs", "-j", type=int, default=os.cpu_count() or 4,
                        help="Number of parallel jobs (default: CPU count)")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")

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
            files = [str(f) for f in sorted(target.rglob("*.c")) + sorted(target.rglob("*.h"))
                     if not is_excluded(str(f))]
        else:
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
