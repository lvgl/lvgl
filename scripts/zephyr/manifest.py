#!/usr/bin/env python3
"""Operations on the Zephyr pin in ``tests/zephyr/west.yml``.

    manifest.py show
    manifest.py check-version
    manifest.py materialize --output ws/manifest/west.yml \
        [--repo URL] [--revision REV]

``check-version`` always exits 0 and reports "unknown" without network access:
being behind upstream is a warning, never a build failure.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sys
import urllib.error
import urllib.request
from pathlib import Path

RELEASES_URL = "https://api.github.com/repos/zephyrproject-rtos/zephyr/releases?per_page=100"
COMMIT_URL = "https://api.github.com/repos/{slug}/commits/{rev}"

# Final release tags only; a suffix (-rc1, ...) means prerelease.
VERSION_RE = re.compile(r"^v(\d+)\.(\d+)\.(\d+)$")


def parse_version(tag: str) -> tuple[int, int, int] | None:
    m = VERSION_RE.match(tag.strip())
    return (int(m[1]), int(m[2]), int(m[3])) if m else None


def read_pin(manifest: Path) -> tuple[str, str]:
    """Return (url_base/repo, revision) for the ``zephyr`` project.

    Parsed with a small hand-rolled reader rather than PyYAML so that the
    version check has no dependency beyond the standard library.
    """
    text = manifest.read_text()
    url_base = ""
    m = re.search(r"^\s*url-base:\s*(\S+)", text, re.M)
    if m:
        url_base = m.group(1).strip().rstrip("/")
    m = re.search(r"^\s*-\s*name:\s*zephyr\s*$(.*?)(?=^\s*-\s*name:|\Z)", text, re.M | re.S)
    if not m:
        raise SystemExit(f"{manifest}: no project named 'zephyr' found")
    rev = re.search(r"^\s*revision:\s*(\S+)", m.group(1), re.M)
    if not rev:
        raise SystemExit(f"{manifest}: the 'zephyr' project has no revision")
    return f"{url_base}/zephyr", rev.group(1).strip()


def _get_json(url: str):
    headers = {"Accept": "application/vnd.github+json"}
    # The rate limit is per IP, and a CI runner's is shared.
    token = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN")
    if token:
        headers["Authorization"] = f"Bearer {token}"
    req = urllib.request.Request(url, headers=headers)
    with urllib.request.urlopen(req, timeout=30) as resp:
        return json.load(resp)


def latest_release() -> tuple[str, str] | None:
    """Newest final release as (tag, published_at), by semantic version.

    Ordered by version, never by publish date: Zephyr backports to older
    branches, so v4.4.1 can be published before v4.3.1 while still being the
    newer version.
    """
    try:
        releases = _get_json(RELEASES_URL)
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError, OSError):
        return None
    finals = []
    for rel in releases:
        if rel.get("prerelease") or rel.get("draft"):
            continue
        ver = parse_version(rel.get("tag_name", ""))
        if ver:
            finals.append((ver, rel["tag_name"], rel.get("published_at", "")))
    if not finals:
        return None
    finals.sort(reverse=True)
    return finals[0][1], finals[0][2]


def commit_date(repo: str, rev: str) -> str | None:
    slug = repo.removeprefix("https://github.com/")
    try:
        return _get_json(COMMIT_URL.format(slug=slug, rev=rev))["commit"]["committer"]["date"]
    except (urllib.error.URLError, TimeoutError, KeyError, json.JSONDecodeError, OSError):
        return None


def materialize(manifest: Path, output: Path, repo: str = "", revision: str = "") -> tuple[str, str]:
    """Copy *manifest* to *output*, optionally overriding repo and revision.

    Only the two pinned values are rewritten and only on their own lines, so
    the comments in the committed manifest survive into the workspace copy.
    The committed file is never modified.
    """
    text = manifest.read_text()
    if repo:
        # url-base is the remote prefix; repo-path adds 'zephyr'
        base = repo.rstrip("/")
        base = base[: -len("/zephyr")] if base.endswith("/zephyr") else base
        text, n = re.subn(r"^(\s*url-base:\s*)\S+", lambda m: m.group(1) + base, text, count=1, flags=re.M)
        if not n:
            raise SystemExit(f"{manifest}: no url-base to override")
    if revision:
        def sub_rev(match: re.Match) -> str:
            body = re.sub(r"^(\s*revision:\s*)\S+", lambda m: m.group(1) + revision,
                          match.group(2), count=1, flags=re.M)
            return match.group(1) + body
        text, n = re.subn(r"(^\s*-\s*name:\s*zephyr\s*$)(.*?)(?=^\s*-\s*name:|\Z)",
                          sub_rev, text, count=1, flags=re.M | re.S)
        if not n:
            raise SystemExit(f"{manifest}: no project named 'zephyr' to override")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(text)
    return read_pin(output)


def cmd_materialize(args) -> int:
    repo, rev = materialize(Path(args.manifest), Path(args.output), args.repo, args.revision)
    print(f"repo={repo}")
    print(f"revision={rev}")
    return 0


def cmd_show(args) -> int:
    repo, rev = read_pin(Path(args.manifest))
    print(f"repo={repo}")
    print(f"revision={rev}")
    return 0


def cmd_check(args) -> int:
    repo, rev = read_pin(Path(args.manifest))
    latest = latest_release()
    if latest is None:
        print(f"unknown: pinned {rev}, could not reach the GitHub releases API")
        return 0
    tag, published = latest
    pinned = parse_version(rev)

    if pinned is not None:
        newest = parse_version(tag)
        if newest > pinned:
            print(
                f"outdated: pinned {rev}, newest release {tag} "
                f"(released {published[:10]})"
            )
        else:
            print(f"current: pinned {rev} is the newest release")
        return 0

    # A branch or SHA has no version to compare, so fall back to dates.
    date = commit_date(repo, rev)
    if date is None:
        print(f"unknown: pinned {rev} is not a release tag and its date is unavailable")
    elif published > date:
        print(
            f"outdated: pinned {rev} dates from {date[:10]}, "
            f"release {tag} came out later on {published[:10]}"
        )
    else:
        print(f"current: pinned {rev} ({date[:10]}) is newer than release {tag}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name, func in (("show", cmd_show), ("check-version", cmd_check),
                       ("materialize", cmd_materialize)):
        p = sub.add_parser(name)
        p.add_argument("--manifest", default="tests/zephyr/west.yml")
        if name == "materialize":
            p.add_argument("--output", required=True)
            p.add_argument("--repo", default="", help="override the Zephyr repository URL")
            p.add_argument("--revision", default="", help="override the branch, tag or SHA")
        p.set_defaults(func=func)
    args = ap.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
