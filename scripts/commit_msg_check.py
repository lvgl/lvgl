#!/usr/bin/env python3

"""
LVGL Commit Message Style Checker

Format: type(scope): description

Valid types: feat, fix, arch, test, perf, example, refactor, revert, docs, style, chore, ci, build
Scope:       required (except chore/docs/ci), letters/digits/_/-/ allowed, e.g. (draw), (obj)
Description: lowercase start, no trailing period

Usage:
    commit_msg_check.py                     # Check commits between base branch and HEAD
    commit_msg_check.py --base <branch>     # Specify base branch
    commit_msg_check.py --check-title "msg" # Check a single PR title / commit message
    commit_msg_check.py --self-test         # Run self-tests

See: https://docs.lvgl.io/master/contributing/pull_requests.html#commit-message-format
"""

import os
import re
import subprocess
import sys
import argparse

VALID_TYPES = [
    "feat",
    "fix",
    "arch",
    "test",
    "perf",
    "example",
    "refactor",
    "revert",
    "docs",
    "style",
    "chore",
    "ci",
    "build",
]

# Common typos/aliases -> correct type
TYPE_TYPOS = {
    "feature": "feat",
    "fea": "feat",
    "refact": "refactor",
    "doc": "docs",
    "tests": "test",
    "bugfix": "fix",
    "hotfix": "fix",
    "perf_opt": "perf",
    "optimize": "perf",
}

VALID_TYPES_RE = "|".join(VALID_TYPES)

# type(scope): description  (chore/docs/ci allow omitting scope)
FULL_PATTERN = re.compile(rf"^({VALID_TYPES_RE})\(([a-zA-Z0-9_/-]+)\): (.+)$")

# Types that allow omitting scope
SCOPE_OPTIONAL_TYPES = {"chore", "docs", "ci"}

# type: description (no scope, for scope-optional types)
NO_SCOPE_PATTERN = re.compile(rf"^({'|'.join(SCOPE_OPTIONAL_TYPES)}): (.+)$")

# type( or type:
TYPE_ONLY_PATTERN = re.compile(r"^([a-zA-Z_]+)")

# "don't squash" PR pattern (rebase merge, title won't become commit msg)
# Covers: don't squash, dont squash, dont's squash, do not squash, do-not-squash
DONT_SQUASH_PATTERN = re.compile(
    r"(?:don'?t'?s?|do[\s-]+not)[\s-]+squash", re.IGNORECASE
)

# Commit message format documentation URL
COMMIT_MSG_DOC_URL = (
    "https://docs.lvgl.io/master/contributing/pull_requests.html"
    "#commit-message-format"
)


def check_commit_msg(msg):
    """Check a single commit message subject line.

    Returns a list of error strings. Empty list means OK.
    """
    errors = []

    # Strip leading/trailing whitespace (common in git log output)
    msg = msg.strip()

    if not msg:
        errors.append("Commit message is empty")
        return errors

    # Allow Revert commits
    if msg.startswith('Revert "'):
        return errors

    # Allow merge commits
    if msg.startswith("Merge "):
        return errors

    # Allow "don't squash" PRs (rebase merge, title won't become commit msg)
    if DONT_SQUASH_PATTERN.search(msg):
        return errors

    # Check for Chinese punctuation (common mistake)
    cn_punctuation = {
        "\uff08": "(",  # （ -> (
        "\uff09": ")",  # ） -> )
        "\uff1a": ":",  # ： -> :
        "\u3002": ".",  # 。 -> .
        "\uff0c": ",",  # ， -> ,
        "\uff1b": ";",  # ； -> ;
    }
    found_cn = [
        f"'{ch}'(U+{ord(ch):04X}) -> '{en}'"
        for ch, en in cn_punctuation.items()
        if ch in msg
    ]
    if found_cn:
        errors.append(
            f"Chinese punctuation detected: {', '.join(found_cn)}. "
            "Please use English punctuation only"
        )
        return errors

    # Extract type word
    m = TYPE_ONLY_PATTERN.match(msg)
    type_word = m.group(1) if m else ""
    type_lower = type_word.lower()

    # Check common typos first
    if type_lower in TYPE_TYPOS:
        correct = TYPE_TYPOS[type_lower]
        errors.append(f"Type '{type_word}' is not standard, did you mean '{correct}'?")
        return errors

    # Check type is valid and followed by '('
    type_with_paren = re.compile(rf"^({VALID_TYPES_RE})\(")
    if not type_with_paren.match(msg):
        # type: desc (missing scope)
        type_with_colon = re.compile(rf"^({VALID_TYPES_RE}):")
        if type_with_colon.match(msg):
            # Allow scope-optional types (chore, docs, ci) without scope
            if type_lower in SCOPE_OPTIONAL_TYPES:
                no_scope_match = NO_SCOPE_PATTERN.match(msg)
                if no_scope_match:
                    desc = no_scope_match.group(2)
                    if desc and desc[0].isupper():
                        errors.append(
                            f"Description should start with lowercase: '{desc[:30]}...'"
                        )
                    if desc.endswith("."):
                        errors.append("Description should not end with a period")
                    return errors
            errors.append(
                "Missing scope. Use 'type(scope): description' "
                "instead of 'type: description'"
            )
        elif msg and msg[0].isupper():
            errors.append(
                "Do not start with a capital letter. "
                "Use conventional commit format: type(scope): description"
            )
        else:
            errors.append(
                f"Invalid type '{type_word}'. "
                f"Allowed types: {', '.join(VALID_TYPES)}"
            )
        return errors

    # Full format check
    full = FULL_PATTERN.match(msg)
    if not full:
        # Diagnose specific issues
        empty_scope = re.compile(rf"^({VALID_TYPES_RE})\(\)")
        no_space = re.compile(rf"^({VALID_TYPES_RE})\([^)]*\):[^ ]")
        no_colon = re.compile(rf"^({VALID_TYPES_RE})\([^)]*\)[^:]")

        if empty_scope.match(msg):
            errors.append("Scope cannot be empty")
        elif no_space.match(msg):
            errors.append("Missing space after colon. Use 'type(scope): description'")
        elif no_colon.match(msg):
            errors.append("Missing colon after scope. Use 'type(scope): description'")
        else:
            # Check if scope contains filename or PR reference
            scope_match = re.match(rf"^({VALID_TYPES_RE})\(([^)]+)\): .+", msg)
            if scope_match:
                scope = scope_match.group(2)
                if re.search(r"\.[a-zA-Z]+$", scope):
                    errors.append(
                        f"Scope '{scope}' looks like a filename. "
                        "Use a module name instead, e.g. 'obj' not 'lv_obj.h'"
                    )
                elif "#" in scope:
                    errors.append(
                        f"Scope '{scope}' should be a module name, "
                        "not a PR reference. Put PR number in description, "
                        "e.g. 'fix(module): description (#1234)'"
                    )
                else:
                    errors.append("Invalid format. Expected: type(scope): description")
            else:
                errors.append("Invalid format. Expected: type(scope): description")
        return errors

    # Validate description
    desc = full.group(3)

    if desc and desc[0].isupper():
        errors.append(f"Description should start with lowercase: '{desc[:30]}...'")

    if desc.endswith("."):
        errors.append("Description should not end with a period")

    return errors


def git_run(*args):
    """Run a git command and return stdout."""
    result = subprocess.run(["git"] + list(args), capture_output=True, text=True)
    return result.stdout.strip(), result.returncode


def find_base_branch():
    """Find the base branch for comparison."""
    candidates = ["vela/dev-graphic", "m/dev-graphic", "mainline/master"]
    for branch in candidates:
        _, rc = git_run("rev-parse", "--verify", branch)
        if rc == 0:
            return branch
    return None


def check_commits(base_branch=None, last_n=None):
    """Check all commits between base branch and HEAD."""
    if last_n:
        log_output, _ = git_run("log", "--oneline", f"-{last_n}")
        if not log_output:
            print("No commits to check.")
            return 0
        print(f"Checking commit message style (last {last_n} commits)...")
    else:
        if not base_branch:
            base_branch = find_base_branch()
            if not base_branch:
                print(
                    "Warning: Could not determine base branch, "
                    "skipping commit message style check."
                )
                return 0

        merge_base, rc = git_run("merge-base", base_branch, "HEAD")
        if rc != 0 or not merge_base:
            print(
                "Warning: Could not determine merge base, "
                "skipping commit message style check."
            )
            return 0

        head, _ = git_run("rev-parse", "HEAD")
        if head == merge_base:
            print("No new commits to check, skipping commit message style check.")
            return 0

        log_output, _ = git_run("log", "--oneline", f"{merge_base}..HEAD")
        if not log_output:
            print("No new commits to check.")
            return 0

        print(f"Checking commit message style (base: {base_branch})...")

    error_count = 0
    total_count = 0
    for line in log_output.splitlines():
        parts = line.split(" ", 1)
        if len(parts) < 2:
            continue
        hash_str, msg = parts[0], parts[1]
        total_count += 1
        errors = check_commit_msg(msg)
        if errors:
            error_count += 1
            print(f"\n  Commit: {hash_str[:12]} {msg}")
            for e in errors:
                print(f"  ✗ {e}")
        else:
            print(f"  ✓ {hash_str[:12]} {msg}")

    if error_count > 0:
        print(
            f"""
==========================================
 Commit message style check FAILED
 {error_count} commit(s) with bad format
==========================================

Expected format: type(scope): description

  Valid types: {', '.join(VALID_TYPES)}
  Scope:       required (except chore/docs/ci), e.g. (draw), (obj), (style)
  Description: lowercase start, no trailing period

Good examples:
  feat(draw): add new gradient support
  fix(obj): fix crash when object is deleted
  test(cache): add complete cache test cases
  perf(style): optimize style removal performance

See: {COMMIT_MSG_DOC_URL}

Use 'git commit --amend' or 'git rebase -i' to fix your commit messages."""
        )
        return 1

    print("Commit message style check passed")
    return 0


# ============================================================================
# Self-test
# ============================================================================


def self_test():
    """Run self-tests to verify the checker works correctly."""

    pass_cases = [
        # Standard format
        ("feat(draw): add support for dashed line rendering", "standard feat"),
        ("fix(render): resolve null pointer in flush callback", "standard fix"),
        ("test(cache): add unit tests for eviction policy", "standard test"),
        ("perf(style): reduce redundant lookups in style resolve", "standard perf"),
        ("refactor(obj): split tree logic into separate module", "standard refactor"),
        ("docs(readme): update build instructions for linux", "standard docs"),
        ("style(src): apply clang-format to all source files", "standard style"),
        ("chore(deps): bump third-party library to latest tag", "standard chore"),
        ("ci(github): add workflow for automated testing", "standard ci"),
        ("build(cmake): add option to disable example targets", "standard build"),
        ("arch(core): restructure module dependency graph", "standard arch"),
        ("example(widgets): add demo for new button styles", "standard example"),
        # Scope with special chars
        ("fix(mod_a/sub_b): handle edge case in sub module", "scope with slash"),
        ("fix(draw-sw): prevent buffer overflow in blend op", "scope with hyphen"),
        ("feat(widget_v2): expose new public api for widgets", "scope with underscore"),
        ("feat(myGFX): add hardware acceleration support", "scope with uppercase"),
        # Revert / Merge (always allowed)
        ('Revert "fix(render): disable fast path for now"', "revert commit"),
        ("Merge branch 'main' into dev", "merge commit"),
        ("revert(render): undo fast path optimization change", "revert with scope"),
        # Edge: description exactly 10 chars
        ("feat(core): 0123456789", "description exactly 10 chars"),
        # PR number in description
        ("fix(render): handle opacity reset on layer fail (#9521)", "with PR number"),
        # chore/docs/ci without scope (allowed)
        ("chore: bump version to release candidate tag", "chore without scope"),
        ("chore: fix typos in configuration file names", "chore without scope 2"),
        ("docs: fix typos", "docs without scope"),
        ("docs: add hero image", "docs without scope 2"),
        ("ci: add workflow for automated testing", "ci without scope"),
        ("ci: deploy doc builds to release folders", "ci without scope 2"),
        # Don't squash variants (rebase merge, skip title check)
        ("Dont's squash: minor docs fixes", "dont squash variant 1"),
        (
            "Dont' Squash: improvements and fixes to workflow",
            "dont squash variant 2",
        ),
        ("Dont's squash - gradient updates", "dont squash variant 3"),
        (
            "feat(gdb): add lvgl GDB plugin (don't squash)",
            "dont squash in parens",
        ),
        ("DONT SQUASH: feat(draw): add new gradient support", "dont squash caps"),
        ("dont squash - multiple independent fixes", "dont squash lowercase"),
        ("do not squash: multiple independent fixes", "do not squash"),
        ("do-not-squash: multiple independent fixes", "do-not-squash"),
        # Leading/trailing whitespace (should be stripped)
        ("  feat(draw): add gradient support  ", "leading/trailing spaces"),
    ]

    fail_cases = [
        # Common typos
        ("feature(anim): add transition support for opacity", "typo: feature -> feat"),
        ("fea(scroll): add force elastic attribute to scroll", "typo: fea -> feat"),
        (
            "refact(gif): restructure decoder and add testcase",
            "typo: refact -> refactor",
        ),
        ("doc(readme): update build instructions for linux", "typo: doc -> docs"),
        ("tests(cache): add unit tests for eviction policy", "typo: tests -> test"),
        ("bugfix(render): resolve null pointer in callback", "typo: bugfix -> fix"),
        ("hotfix(obj): fix crash when object is deleted", "typo: hotfix -> fix"),
        # Missing scope
        ("fix: handle invalid escape sequence in parser", "missing scope"),
        ("feat: add something really cool to the project", "missing scope"),
        ("test: add unit tests without specifying a scope", "missing scope test"),
        # Capital letter start (no type)
        ("Add new parameter for module initialization", "capital letter start"),
        ("Update documentation with new build instructions", "capital letter start"),
        # Invalid type
        ("update(core): change default config values here", "invalid type"),
        ("add(draw): introduce new gradient for objects", "invalid type"),
        ("wayland: add API to get fullscreen state", "invalid type module name"),
        # Empty scope
        ("feat(): add something without a scope name", "empty scope"),
        # Missing space after colon
        ("feat(draw):implement dashed line rendering now", "no space after colon"),
        ("fix(parser):handle edge case in token scanner", "no space after colon 2"),
        # Scope is a filename (not allowed)
        ("fix(helper_sw.c): prevent buffer overflow in blend", "scope is filename .c"),
        ("feat(lv_obj.h): expose new public api for widgets", "scope is filename .h"),
        ("docs(README.md): update build instructions for dev", "scope is filename .md"),
        # Scope is a PR reference (not allowed)
        (
            "fix(PR#1234): address review comments from reviewer",
            "scope is PR reference",
        ),
        # Space before colon
        ("fix(parser) :handle edge case in token scanner", "space before colon"),
        # Missing colon
        ("feat(draw) implement dashed line rendering now", "no colon after scope"),
        # Description starts with uppercase
        ("feat(draw): Implement dashed line rendering now", "uppercase description"),
        ("fix(scale): Don't return early on main drawing", "uppercase contraction"),
        (
            "chore(cmsis-pack): Prepare for v9.5.0",
            "uppercase description chore with scope",
        ),
        # Description ends with period
        ("feat(draw): implement dashed line rendering now.", "trailing period"),
        (
            "feat(nema_gfx): integrate hardware acceleration.",
            "trailing period 2",
        ),
        # Chinese punctuation
        ("fix\uff08draw\uff09: handle edge case in flush callback", "chinese parens"),
        ("fix(draw)\uff1ahandle edge case in flush callback", "chinese colon"),
        ("fix(draw): handle edge case in flush callback\u3002", "chinese period"),
        (
            "fix(draw)\uff1a handle edge case in flush callback",
            "chinese colon with space",
        ),
        (
            "feat\uff08core\uff09\uff1aadd new public api for widget tree",
            "all chinese punctuation",
        ),
        # Random garbage
        ("this is not a valid commit message at all", "random text"),
        ("just some random words without any structure", "random text 2"),
        ("WIP something something something something", "WIP commit"),
        ("Feat/lv check obj", "branch name as title"),
        ("Initial commit", "initial commit"),
        # Empty message
        ("", "empty message"),
        ("   ", "whitespace only"),
    ]

    passed = 0
    failed = 0
    total = len(pass_cases) + len(fail_cases)

    print("=" * 60)
    print(" Commit Message Checker Self-Test")
    print("=" * 60)

    # Test cases that should PASS
    print("\n--- Should PASS ---")
    for msg, desc in pass_cases:
        errors = check_commit_msg(msg)
        if not errors:
            passed += 1
            print(f"  ✓ PASS  [{desc}]")
        else:
            failed += 1
            print(f"  ✗ FAIL  [{desc}]")
            print(f"          msg: {msg}")
            for e in errors:
                print(f"          err: {e}")

    # Test cases that should FAIL
    print("\n--- Should FAIL ---")
    for msg, desc in fail_cases:
        errors = check_commit_msg(msg)
        if errors:
            passed += 1
            print(f"  ✓ PASS  [{desc}] -> caught: {errors[0]}")
        else:
            failed += 1
            print(f"  ✗ FAIL  [{desc}] -> should have been rejected!")
            print(f"          msg: {msg}")

    print(f"\n{'=' * 60}")
    print(f" Results: {passed}/{total} passed, {failed} failed")
    print(f"{'=' * 60}")

    # Lint self
    print(f"\n{'=' * 60}")
    print(" Lint Check (self)")
    print(f"{'=' * 60}")

    script_path = os.path.abspath(__file__)
    lint_failed = False

    # Syntax check
    try:
        import py_compile

        py_compile.compile(script_path, doraise=True)
        print("  ✓ py_compile: syntax OK")
    except py_compile.PyCompileError as e:
        print(f"  ✗ py_compile: {e}")
        lint_failed = True

    # flake8 if available
    try:
        result = subprocess.run(
            ["flake8", "--max-line-length=120", "--ignore=E501,W503", script_path],
            capture_output=True,
            text=True,
        )
        if result.returncode == 0:
            print("  ✓ flake8: no issues")
        else:
            print("  ✗ flake8:")
            for line in result.stdout.strip().splitlines():
                print(f"    {line}")
            lint_failed = True
    except FileNotFoundError:
        print("  - flake8: not installed, skipped")

    if lint_failed:
        failed += 1

    return 0 if (failed == 0 and not lint_failed) else 1


def check_title(title):
    """Check a single PR title / commit message string."""
    errors = check_commit_msg(title)
    if errors:
        print(f"\n  PR Title: {title}")
        for e in errors:
            print(f"  ✗ {e}")
        print(
            f"\nExpected format: type(scope): description\n"
            f"\n"
            f"  Valid types: {', '.join(VALID_TYPES)}\n"
            f"  Scope:       required (except chore/docs/ci), e.g. (draw), (obj)\n"
            f"  Description: lowercase start, no trailing period\n"
            f"\n"
            f"Examples:\n"
            f"  feat(draw): add new gradient support\n"
            f"  fix(obj): fix crash when object is deleted\n"
            f"\n"
            f"See: {COMMIT_MSG_DOC_URL}"
        )
        return 1
    print(f"✓ PR title OK: {title}")
    return 0


def main():
    parser = argparse.ArgumentParser(description="LVGL Commit Message Style Checker")
    parser.add_argument(
        "--self-test", action="store_true", help="Run self-tests to verify the checker"
    )
    parser.add_argument(
        "--base",
        type=str,
        default=None,
        help="Base branch for comparison (auto-detected if omitted)",
    )
    parser.add_argument(
        "--last",
        type=int,
        default=None,
        help="Check the last N commits (useful for local testing)",
    )
    parser.add_argument(
        "--check-title",
        type=str,
        default=None,
        help="Check a single PR title / commit message string",
    )
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    if args.check_title is not None:
        return check_title(args.check_title)

    return check_commits(args.base, args.last)


if __name__ == "__main__":
    sys.exit(main())
