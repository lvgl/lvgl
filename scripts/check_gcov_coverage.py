#!/usr/bin/env python3
"""
Check code coverage for new lines in a git commit using gcovr
"""
import argparse
import subprocess
import json
import re
import sys
from typing import Dict, Set, Tuple, List


def create_argument_parser() -> argparse.ArgumentParser:
    """Create argument parser"""
    parser = argparse.ArgumentParser(
        description="Check gcov coverage for new code in a git commit",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""  
Examples:  
  %(prog)s HEAD                    # Check latest commit  
  %(prog)s abc123def               # Check specific commit  
  %(prog)s HEAD --root /path/to/project  
  %(prog)s HEAD --gcovr-args "--filter src/"  
        """,
    )

    parser.add_argument(
        "commit", help="Git commit hash or reference (e.g. HEAD, HEAD~1)"
    )

    parser.add_argument(
        "--root",
        "-r",
        default=".",
        help="Project root directory (default: current directory)",
    )

    parser.add_argument(
        "--gcovr-args",
        default="",
        help="Additional arguments to pass to gcovr (e.g. '--filter src/ --exclude test/')",
    )

    parser.add_argument(
        "--verbose", "-v", action="store_true", help="Show verbose output"
    )

    parser.add_argument(
        "--fail-under",
        type=float,
        metavar="PERCENT",
        help="Fail if coverage is below this percentage (0-100)",
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


def get_coverage_data(root: str, extra_args: str) -> Dict[str, Dict[int, int]]:
    """
    Get coverage data using gcovr
    Returns: {file_path: {line_number: execution_count}}
    Raises: subprocess.CalledProcessError if gcovr fails
    """
    cmd = ["gcovr", "--gcov-ignore-parse-errors", "--json", "-", "--root", root]
    if extra_args:
        cmd.extend(extra_args.split())

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

    return coverage_data


def check_commit_coverage(
    commit: str, root: str, gcovr_args: str, verbose: bool
) -> Tuple[int, int, List[Tuple[str, int]]]:
    """
    Check coverage for a commit
    Returns: (covered_lines, total_new_lines, uncovered_lines)
    """
    if verbose:
        print(f"Analyzing commit {commit}...")

    changed_lines = get_changed_lines(commit, root)
    if verbose:
        print(f"Found changes in {len(changed_lines)} files")

    if verbose:
        print("Getting coverage data...")
    coverage_data = get_coverage_data(root, gcovr_args)

    total_new_lines = 0
    covered_lines = 0
    uncovered_lines: List[Tuple[str, int]] = []

    for filename, line_numbers in changed_lines.items():
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
        covered, total, uncovered = check_commit_coverage(
            args.commit, args.root, args.gcovr_args, args.verbose
        )

        # Print results
        print("\n" + "=" * 60)
        print(f"Coverage analysis results for commit {args.commit}:")
        print("=" * 60)
        print(f"New lines of code: {total}")
        print(f"Covered lines: {covered}")
        print(f"Uncovered lines: {len(uncovered)}")

        if total > 0:
            coverage_percent = (covered / total) * 100
            print(f"Coverage: {coverage_percent:.2f}%")

            # Check if coverage meets minimum requirement
            if args.fail_under is not None and coverage_percent < args.fail_under:
                print(
                    f"\n✗ Coverage {coverage_percent:.2f}% is below required {args.fail_under}%"
                )
                return 2

        if uncovered:
            print("\nUncovered lines:")
            for filename, lineno in sorted(uncovered):
                print(f"  {filename}:{lineno}")
            return 1
        else:
            print("\n✓ All new code is covered!")
            return 0

    except subprocess.CalledProcessError as e:
        print(f"Error: Command failed - {e}", file=sys.stderr)
        if e.stderr:
            print(f"Error details:\n{e.stderr}", file=sys.stderr)
        if e.stdout:
            print(f"Command output:\n{e.stdout}", file=sys.stderr)
        return e.returncode if e.returncode is not None else 64
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
