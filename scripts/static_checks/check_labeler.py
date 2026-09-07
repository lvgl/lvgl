#!/usr/bin/env python3
"""
Check .github/labeler.yml against the repository.

Modes:
  globs: Every glob still matches a file in the repository
  labels-exist: Every label in the config exists on GitHub (needs network)
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import urllib.error
import urllib.request
from pathlib import Path

import yaml

SCRIPT_PATH = Path(__file__).resolve()
LVGL_ROOT_DIR = SCRIPT_PATH.parents[2]
LABELER_YML = LVGL_ROOT_DIR / ".github" / "labeler.yml"

# The match keys actions/labeler understands for the `changed-files` matcher.
GLOB_KEYS = (
    "any-glob-to-any-file",
    "any-glob-to-all-files",
    "all-globs-to-any-file",
    "all-globs-to-all-files",
)

# Matchers that take branch-name regexes instead of path globs.
BRANCH_MATCHERS = ("base-branch", "head-branch")

# Rule keys that group other rules instead of matching anything themselves.
GROUP_KEYS = ("any", "all")

RESERVED_KEYS = ("max-files-changed", "changed-files-labels-limit")


# ---------------------------------------------------------------------------
# Glob matching (minimatch semantics, as used by actions/labeler)
# ---------------------------------------------------------------------------


def glob_to_regex(pattern: str) -> re.Pattern:
    """
    Translate a minimatch glob into an anchored regex.

    `*` and `?` never cross a path separator, `**` matches whole segments.
    """
    if "{" in pattern:
        raise ValueError(f"brace expansion is not supported by this check: {pattern}")

    out: list[str] = []
    i, n = 0, len(pattern)

    while i < n:
        if pattern.startswith("**/", i):
            out.append("(?:[^/]+/)*")
            i += 3
        elif pattern.startswith("/**", i) and i + 3 == n:
            out.append("(?:/.*)?")
            i += 3
        elif pattern.startswith("**", i):
            out.append(".*")
            i += 2
        elif pattern[i] == "*":
            out.append("[^/]*")
            i += 1
        elif pattern[i] == "?":
            out.append("[^/]")
            i += 1
        elif pattern[i] == "[":
            j = i + 1
            if j < n and pattern[j] in "!^":
                j += 1
            if j < n and pattern[j] == "]":
                j += 1
            while j < n and pattern[j] != "]":
                j += 1
            if j >= n:
                out.append(re.escape("["))
                i += 1
            else:
                inner = pattern[i + 1 : j]
                if inner and inner[0] in "!^":
                    inner = "^" + inner[1:]
                out.append(f"[{inner}]")
                i = j + 1
        else:
            out.append(re.escape(pattern[i]))
            i += 1

    return re.compile("^" + "".join(out) + "$")


def tracked_files(repo_root: Path) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files"],
        cwd=repo_root,
        capture_output=True,
        text=True,
        check=True,
    )
    return [line for line in result.stdout.splitlines() if line]


# ---------------------------------------------------------------------------
# Parsing
# ---------------------------------------------------------------------------


def collect_globs(config: dict) -> tuple[list[tuple[str, str]], list[str]]:
    """
    Return ([(label, glob), ...], [shape error, ...]) for the whole config.
    """
    globs: list[tuple[str, str]] = []
    errors: list[str] = []

    for label, rules in config.items():
        if label in RESERVED_KEYS:
            if not isinstance(rules, int) or isinstance(rules, bool) or rules < 0:
                errors.append(f"{label}: must be a non-negative integer, got {rules!r}")
            continue

        if not isinstance(rules, list):
            errors.append(
                f"{label}: expected a list of rules, got {type(rules).__name__}"
            )
            continue

        found = False
        for rule in rules:
            if not isinstance(rule, dict):
                errors.append(f"{label}: expected a mapping in the rule list")
                continue

            for matcher, matches in rule.items():
                if matcher in GROUP_KEYS:
                    # 'any'/'all' hold a list of ordinary rules; recurse.
                    nested_globs, nested_errors = collect_globs({label: matches})
                    globs.extend(nested_globs)
                    errors.extend(nested_errors)
                    if nested_globs:
                        found = True
                    continue

                if matcher in BRANCH_MATCHERS:
                    patterns = [matches] if isinstance(matches, str) else matches
                    for pattern in patterns:
                        try:
                            re.compile(pattern)
                        except re.error as e:
                            errors.append(
                                f"{label}: invalid {matcher} regex '{pattern}': {e}"
                            )
                    found = True
                    continue
                if matcher != "changed-files":
                    errors.append(f"{label}: unknown matcher '{matcher}'")
                    continue
                for match in matches:
                    for key, patterns in match.items():
                        if key not in GLOB_KEYS:
                            errors.append(f"{label}: unknown match key '{key}'")
                            continue
                        if isinstance(patterns, str):
                            patterns = [patterns]
                        for pattern in patterns:
                            globs.append((label, pattern))
                            found = True

        if not found:
            errors.append(f"{label}: no globs or branch patterns defined")

    return globs, errors


# ---------------------------------------------------------------------------
# Checking
# ---------------------------------------------------------------------------


def check_labeler(repo_root: Path, labeler_yml: Path) -> bool:
    if not labeler_yml.is_file():
        print(f"ERROR: {labeler_yml} not found")
        return False

    config = yaml.safe_load(labeler_yml.read_text(encoding="utf-8"))
    if not isinstance(config, dict):
        print(f"ERROR: {labeler_yml} must be a mapping of label -> rules")
        return False

    globs, errors = collect_globs(config)

    files = tracked_files(repo_root)
    dead: list[tuple[str, str]] = []

    for label, pattern in globs:
        try:
            regex = glob_to_regex(pattern.lstrip("!"))
        except ValueError as e:
            errors.append(f"{label}: {e}")
            continue
        if not any(regex.match(f) for f in files):
            dead.append((label, pattern))

    if errors:
        print(f"MALFORMED rules in {labeler_yml.name} ({len(errors)}):\n")
        for error in errors:
            print(f"  {error}")
        print()

    if dead:
        print(
            f"DEAD globs in {labeler_yml.name} "
            f"({len(dead)} of {len(globs)}). They match no tracked file:\n"
        )
        width = max(len(label) for label, _ in dead)
        for label, pattern in dead:
            print(f"  {label:<{width}}  {pattern}")
        print("\nRemove the rule, or point it at where the files moved to.")
        return False

    if errors:
        return False

    print(
        f"OK: all {len(globs)} globs across "
        f"{len(set(config) - set(RESERVED_KEYS))} labels "
        f"match at least one tracked file"
    )
    return True


# ---------------------------------------------------------------------------
# Label existence
# ---------------------------------------------------------------------------

GITHUB_API = "https://api.github.com"


def repo_slug(repo_root: Path) -> str:
    """Return 'owner/repo', from the Actions environment or the git remote."""
    slug = os.environ.get("GITHUB_REPOSITORY")
    if slug:
        return slug

    url = subprocess.run(
        ["git", "remote", "get-url", "origin"],
        cwd=repo_root,
        capture_output=True,
        text=True,
        check=True,
    ).stdout.strip()

    match = re.search(r"[:/]([^/:]+/[^/]+?)(?:\.git)?$", url)
    if not match:
        raise RuntimeError(f"cannot derive owner/repo from remote url: {url}")
    return match.group(1)


def fetch_labels(slug: str) -> set[str]:
    """Return every label name defined on the repository."""
    headers = {
        "Accept": "application/vnd.github+json",
        "User-Agent": "lvgl-check-labeler",
    }
    token = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN")
    if token:
        headers["Authorization"] = f"Bearer {token}"

    names: set[str] = set()
    page = 1
    while True:
        url = f"{GITHUB_API}/repos/{slug}/labels?per_page=100&page={page}"
        request = urllib.request.Request(url, headers=headers)
        with urllib.request.urlopen(request, timeout=30) as response:
            batch = json.load(response)
        if not batch:
            break
        names.update(item["name"] for item in batch)
        page += 1

    return names


def check_labels_exist(
    repo_root: Path, labeler_yml: Path, slug: str | None = None
) -> bool:
    config = yaml.safe_load(labeler_yml.read_text(encoding="utf-8"))
    if not isinstance(config, dict):
        print(f"ERROR: {labeler_yml} must be a mapping of label -> rules")
        return False

    used = set(config) - set(RESERVED_KEYS)

    try:
        slug = slug or repo_slug(repo_root)
        defined = fetch_labels(slug)
    except (urllib.error.URLError, RuntimeError, subprocess.CalledProcessError) as e:
        print(f"ERROR: could not read the repository labels: {e}")
        return False

    missing = sorted(used - defined)
    if missing:
        print(
            f"MISSING labels on {slug} ({len(missing)} of {len(used)}) - "
            f"actions/labeler cannot apply them:\n"
        )
        for label in missing:
            print(f"  {label}")
        print("\nCreate them with: gh label create '<name>' --color <hex>")
        return False

    print(f"OK: all {len(used)} labels in {labeler_yml.name} exist on {slug}")
    return True


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(
        description="Check .github/labeler.yml against the repository"
    )
    parser.add_argument(
        "mode",
        nargs="?",
        default="globs",
        choices=("globs", "labels-exist"),
        help="which check to run (default: globs)",
    )
    parser.add_argument(
        "--root",
        default=str(LVGL_ROOT_DIR),
        help="Repository root (default: the LVGL root next to this script)",
    )
    parser.add_argument(
        "--repo",
        default=None,
        help="owner/repo to read labels from (labels-exist mode; default: "
        "$GITHUB_REPOSITORY, else the origin remote)",
    )
    parser.add_argument(
        "--labeler",
        default=None,
        help="Path to labeler.yml (default: <root>/.github/labeler.yml)",
    )
    args = parser.parse_args()

    repo_root = Path(args.root).resolve()
    labeler_yml = (
        Path(args.labeler).resolve()
        if args.labeler
        else repo_root / ".github" / "labeler.yml"
    )

    if args.mode == "labels-exist":
        passed = check_labels_exist(repo_root, labeler_yml, args.repo)
    else:
        passed = check_labeler(repo_root, labeler_yml)

    sys.exit(0 if passed else 1)


if __name__ == "__main__":
    main()
