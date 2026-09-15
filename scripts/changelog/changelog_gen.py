#!/usr/bin/env python3
"""Generate a release changelog page from the git history.

Usage:
    python3 changelog_gen.py v9.6.0

Writes docs/src/changelog/CHANGELOG_<version>.mdx.
"""

import argparse
import json
import re
import subprocess
import sys
from datetime import date
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DOCS_DIR = REPO_ROOT / "docs" / "src" / "changelog"
DEFAULT_REPO_URL = "https://github.com/lvgl/lvgl"

TAG_PATTERN = re.compile(r"^v\d+\.\d+\.\d+$")

SUBJECT_WITH_COLON = re.compile(
    r"^(?P<type>[a-zA-Z_]+)(?P<bang1>!)?(?:\((?P<scope>[^)]*)\))?(?P<bang2>!)?:\s*(?P<desc>.+)$"
)

SUBJECT_NO_COLON = re.compile(
    r"^(?P<type>[a-zA-Z_]+)(?P<bang1>!)?\((?P<scope>[^)]*)\)(?P<bang2>!)?\s+(?P<desc>.+)$"
)

TRAILING_PR = re.compile(r"\s*\(#\d+\)\s*$")

# Section order, and the commit types each one collects.
SECTIONS = [
    ("Breaking Changes", None),  # populated by the breaking flag, not by type
    ("Architectural", ["arch"]),
    ("New Features", ["feat"]),
    ("Fixes", ["fix"]),
    ("Performance", ["perf"]),
    ("Examples", ["example", "examples"]),
    ("Docs", ["doc", "docs"]),
    ("Build", ["build"]),
    ("Refactoring", ["refactor", "style"]),
    ("Chores (Scripts & Others)", ["chore"]),
    ("CI and tests", ["ci", "test", "tests"]),
    ("Others", ["revert"]),
]
CATCH_ALL = "Others"

TYPE_TO_SECTION = {t: name for name, types in SECTIONS if types for t in types}

# Subjects that are noise rather than a change worth listing.
IGNORED = re.compile(
    r"^(fix conflict|fix warning|bump version|release )", re.IGNORECASE
)


def display(path):
    try:
        return path.relative_to(REPO_ROOT)
    except ValueError:
        return path


def confirm_overwrite(path, force):
    """The page is meant to be hand-edited after generation, so never clobber it silently."""
    if force or not path.exists():
        return
    if not sys.stdin.isatty():
        sys.exit(f"error: {display(path)} already exists; pass --force to overwrite")
    answer = input(f"{display(path)} already exists. Overwrite? [y/N] ").strip().lower()
    if answer not in ("y", "yes"):
        sys.exit("aborted")


def escape_mdx(text):
    """Neutralise characters MDX would read as JSX or as an expression."""
    text = text.replace("&", "&amp;")
    text = text.replace("<", "&lt;").replace(">", "&gt;")
    return text.replace("{", "&#123;").replace("}", "&#125;")


def git(*args):
    out = subprocess.run(
        ["git", "-C", str(REPO_ROOT), *args],
        check=True,
        capture_output=True,
        text=True,
    )
    return out.stdout


def previous_version_tag():
    """Latest vX.Y.Z tag reachable from HEAD."""
    tags = git("tag", "--list", "--merged", "HEAD", "--sort=-v:refname").split()
    for tag in tags:
        if TAG_PATTERN.match(tag):
            return tag
    sys.exit("error: no vX.Y.Z tag found in the history")


def parse_subject(subject):
    """Split a commit subject into (type, breaking, text). type is None if unparseable."""
    match = SUBJECT_WITH_COLON.match(subject) or SUBJECT_NO_COLON.match(subject)
    if not match:
        return None, False, subject
    breaking = bool(match.group("bang1") or match.group("bang2"))
    return match.group("type").lower(), breaking, match.group("desc")


def collect(revision_range):
    """Return {section: [(hash, subject)]} for every commit in the range."""
    # %x00/%x01 are expanded by git itself; NUL cannot be passed in argv.
    log = git(
        "log",
        revision_range,
        "--no-merges",
        "--reverse",
        "--format=%H%x00%s%x00%b%x01",
    )

    sections = {name: [] for name, _ in SECTIONS}
    for record in log.split("\x01"):
        record = record.strip("\n")
        if not record:
            continue
        commit_hash, subject, body = record.split("\x00", 2)

        # Subjects sometimes carry a stray leading space, and the squash suffix
        # "(#1234)" is redundant once we link the commit itself.
        subject = TRAILING_PR.sub("", subject.strip())
        if not subject or IGNORED.match(subject):
            continue

        commit_type, bang, _ = parse_subject(subject)
        breaking = bang or bool(
            re.search(r"^BREAKING[ -]CHANGE:\s*", body, re.MULTILINE)
        )
        if breaking:
            section = "Breaking Changes"
        else:
            section = TYPE_TO_SECTION.get(commit_type, CATCH_ALL)
        sections[section].append((commit_hash, subject))

    return sections


def render(version, when, sections, repo_url, previous_tag):
    lines = [
        "---",
        f"title: {version} Change Log",
        f"description: Summary of the changes of LVGL {version}",
        "---",
        "",
        f"## {version} {when}",
        "",
    ]

    for name, _ in SECTIONS:
        entries = sections[name]
        if not entries:
            continue
        lines.append(f"### {name}")
        lines.append("")
        for commit_hash, subject in entries:
            link = f"{repo_url}/commit/{commit_hash}"
            lines.append(f"- [{commit_hash[:7]}]({link}) - {escape_mdx(subject)}")
        lines.append("")

    compare = f"{repo_url}/compare/{previous_tag}...{version}"
    lines.append(f"<sub>Full changes [here]({compare})</sub>")
    lines.append("")
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("version", help="version being released, e.g. v9.6.0")
    parser.add_argument(
        "--since", help="tag to start from (default: latest vX.Y.Z tag)"
    )
    parser.add_argument(
        "--repo-url",
        default=DEFAULT_REPO_URL,
        help=f"base URL used for links (default: {DEFAULT_REPO_URL})",
    )
    parser.add_argument("--date", help="release date (default: today)")
    parser.add_argument(
        "-o",
        "--output",
        help="output file (default: docs/src/changelog/CHANGELOG_<version>.mdx)",
    )
    parser.add_argument(
        "-f",
        "--force",
        action="store_true",
        help="overwrite the output file without asking",
    )
    args = parser.parse_args()

    output = (
        Path(args.output) if args.output else DOCS_DIR / f"CHANGELOG_{args.version}.mdx"
    )
    confirm_overwrite(output, args.force)

    previous_tag = args.since or previous_version_tag()
    when = args.date or date.today().strftime("%d %B %Y").lstrip("0")

    sections = collect(f"{previous_tag}..HEAD")
    total = sum(len(v) for v in sections.values())
    if total == 0:
        sys.exit(f"error: no commits between {previous_tag} and HEAD")

    output.write_text(
        render(args.version, when, sections, args.repo_url.rstrip("/"), previous_tag)
    )

    print(f"{display(output)}: {total} commits since {previous_tag}")
    for name, _ in SECTIONS:
        if sections[name]:
            print(f"  {len(sections[name]):4d}  {name}")

    meta = DOCS_DIR / "meta.json"
    page = output.stem
    if meta.exists() and page not in json.loads(meta.read_text()).get("pages", []):
        print(
            f'\nnote: add "{page}" to {meta.relative_to(REPO_ROOT)} to list it in the docs'
        )


if __name__ == "__main__":
    main()
