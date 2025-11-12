#!/usr/bin/env python3
"""
Check code coverage for new lines in a git commit using gcovr
"""
import argparse
import subprocess
import json
import re
import sys
import os
from typing import Dict, Set, Tuple, List


def create_argument_parser() -> argparse.ArgumentParser:
    """Create argument parser"""
    parser = argparse.ArgumentParser(
        description="Check gcov coverage for new code in a git commit",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        "--commit",
        help="Git commit hash, reference (e.g. HEAD, HEAD~1) or range (base...head)",
        default="HEAD",
    )

    parser.add_argument(
        "--fail-under",
        type=float,
        metavar="PERCENT",
        default=0,
        help="Fail if coverage is below this percentage (0-100), default: 0 (no failure)",
    )

    return parser


def run_git_command(args: List[str], cwd: str = ".") -> str:
    """Run git command and return output"""
    result = subprocess.run(
        ["git"] + args, capture_output=True, text=True, cwd=cwd, check=True
    )
    return result.stdout


def get_changed_lines(commit: str, root: str) -> Dict[str, Set[int]]:
    """
    Get changed lines in specified commit
    Returns: {file_path: {set of changed line numbers}}
    """
    diff_output = run_git_command(
        ["diff", f"{commit}^", commit, "--unified=0"], cwd=root
    )

    changed_lines: Dict[str, Set[int]] = {}
    current_file = None

    for line in diff_output.split("\n"):
        if line.startswith("+++ b/"):
            current_file = line[6:]
            if current_file not in changed_lines:
                changed_lines[current_file] = set()

        elif line.startswith("@@"):
            match = re.search(r"\+(\d+)(?:,(\d+))?", line)
            if match and current_file:
                start_line = int(match.group(1))
                count = int(match.group(2)) if match.group(2) else 1
                for lineno in range(start_line, start_line + count):
                    changed_lines[current_file].add(lineno)

    return changed_lines


def get_coverage_data(root: str) -> Tuple[Dict[str, Dict[int, int]], str]:
    """
    Get coverage data using gcovr
    Returns: ({file_path: {line_number: execution_count}}, filter_pattern)
    Raises: subprocess.CalledProcessError if gcovr fails
    """
    filter_pattern = os.path.join(root, r"src/(?:.*/)?lv_.*\.c")
    cmd = [
        "gcovr",
        "--gcov-ignore-parse-errors",
        "--json",
        "-",
        "--root",
        root,
        "--filter",
        filter_pattern,
    ]

    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        cwd=root,
        check=False,  # Don't raise exception automatically
    )

    if result.returncode != 0:
        error_msg = f"gcovr command failed (exit code {result.returncode}):\n"
        if result.stderr:
            error_msg += f"Error output:\n{result.stderr}\n"
        if result.stdout:
            error_msg += f"Standard output:\n{result.stdout}\n"
        raise subprocess.CalledProcessError(
            result.returncode, cmd, result.stdout, result.stderr
        )

    coverage_json = json.loads(result.stdout)
    coverage_data: Dict[str, Dict[int, int]] = {}

    for file_info in coverage_json.get("files", []):
        filename = file_info["file"]
        coverage_data[filename] = {}

        for line_info in file_info.get("lines", []):
            line_number = line_info["line_number"]
            count = line_info["count"]
            coverage_data[filename][line_number] = count

    return coverage_data, filter_pattern


def check_commit_coverage(
    commit: str, root: str
) -> Tuple[int, int, List[Tuple[str, int]]]:
    """
    Check coverage for a commit or range
    Returns: (covered_lines, total_new_lines, uncovered_lines)
    """

    if "..." in commit:
        # Handle range format (base...head)
        base, head = commit.split("...")
        # Get changes for each commit in the range
        commits = run_git_command(["rev-list", f"{base}...{head}"], cwd=root).split()
    else:
        # Handle single commit
        commits = [commit]

    print(f"Found {len(commits)} commits in range:")
    changed_lines = {}
    for c in commits:
        print(f"  {c}")
        cl = get_changed_lines(c, root)
        for f, lines in cl.items():
            if f not in changed_lines:
                changed_lines[f] = set()
            changed_lines[f].update(lines)

    print(f"Found changes in {len(changed_lines)} files (before filtering):")
    for filename, lines in sorted(changed_lines.items()):
        print(f"  {filename}: {len(lines)} lines changed")

    print("Getting coverage data...")
    coverage_data, filter_pattern = get_coverage_data(root)

    # Extract the regex pattern from the filter (remove root path)
    pattern_str = filter_pattern.replace(root, "").lstrip(os.path.sep)
    pattern = re.compile(pattern_str)

    filtered_lines = {
        f: lines for f, lines in changed_lines.items() if pattern.search(f)
    }

    print(f"After filtering, {len(filtered_lines)} files match pattern '{pattern_str}'")

    total_new_lines = 0
    covered_lines = 0
    uncovered_lines: List[Tuple[str, int]] = []

    for filename, line_numbers in filtered_lines.items():
        file_coverage = coverage_data.get(filename, {})

        for lineno in line_numbers:
            total_new_lines += 1

            if lineno in file_coverage and file_coverage[lineno] > 0:
                covered_lines += 1
            else:
                uncovered_lines.append((filename, lineno))

    return covered_lines, total_new_lines, uncovered_lines


def main() -> int:
    """Main entry point"""
    parser = create_argument_parser()
    args = parser.parse_args()

    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        root = os.path.dirname(script_dir)
        os.chdir(root)
        print(f"Current working directory: {root}")

        covered, total, uncovered = check_commit_coverage(args.commit, root)

        # Print results with better formatting
        title = f" Coverage analysis results for commit {args.commit} "
        separator = "=" * len(title)
        print(f"\n{separator}\n{title}\n{separator}")
        print(f"New lines of code: {total}")
        print(f"Covered lines: {covered}")
        print(f"Uncovered lines: {len(uncovered)}")
        retval = 0

        if total > 0:
            coverage_percent = (covered / total) * 100
            print(f"Coverage: {coverage_percent:.2f}%")

            # Check if coverage meets minimum requirement
            if coverage_percent < args.fail_under:
                print(
                    f"\n✗ Coverage {coverage_percent:.2f}% is below required {args.fail_under}%"
                )
                retval = 1

        if uncovered:
            print("\nUncovered lines:")
            for filename, lineno in sorted(uncovered):
                print(f"  {filename}:{lineno}")

            return retval

        print("\n✓ All new code is covered!")
        return retval

    except subprocess.CalledProcessError as e:
        print(f"Error: Command failed - {e}", file=sys.stderr)
        if e.stderr:
            print(f"Error details:\n{e.stderr}", file=sys.stderr)
        if e.stdout:
            print(f"Command output:\n{e.stdout}", file=sys.stderr)
        return e.returncode
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
