#!/usr/bin/env python3
"""Check markdown links in docs/src/.

Two classes of problems are reported:
  1. INTERNAL BROKEN — root-relative links like (/integration/foo) whose
     resolved path is not present in the deployed open-docs sitemap.
  2. LEGACY DOMAIN  — links that still point to docs.lvgl.io/master|latest|...
     (the old Sphinx site). For each, we try to suggest a local file by
     matching the URL's basename against files under docs/src/.

Convention reminder: this repo uses root-relative links (/foo/bar), not
./foo/bar — survey shows 687 vs 1 across docs/src/.
"""
from __future__ import annotations

import re
import sys
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path
from collections import defaultdict

SITEMAP_URL = "https://lvgl.io/docs/open/sitemap.xml"
SITE_PREFIX = "/docs/open"
DOCS_ROOT = Path(__file__).resolve().parents[1] / "docs" / "src"

# [text](url) — non-greedy text, url stops at whitespace, ) or ".
# Skips images (![...]) by requiring no leading '!' (negative lookbehind).
MD_LINK = re.compile(r"(?<!\!)\[([^\]]*?)\]\(\s*([^\s)]+)(?:\s+\"[^\"]*\")?\s*\)")

# Matches the legacy Sphinx-site domain in any of its forms.
LEGACY_HOST = re.compile(
    r"^https?://(?:docs\.lvgl\.io|lvgl\.io/docs)/(?:master|latest|v\d[^/]*)/",
    re.IGNORECASE,
)


def fetch_sitemap_paths() -> set[str]:
    req = urllib.request.Request(SITEMAP_URL, headers={"User-Agent": "lvgl-link-checker/1.0"})
    with urllib.request.urlopen(req, timeout=30) as resp:
        data = resp.read()
    ns = {"s": "http://www.sitemaps.org/schemas/sitemap/0.9"}
    root = ET.fromstring(data)
    paths = set()
    for loc in root.findall(".//s:url/s:loc", ns):
        url = (loc.text or "").strip()
        # strip scheme+host -> just the path
        path = re.sub(r"^https?://[^/]+", "", url)
        paths.add(path.rstrip("/") or "/")
    return paths


def to_site_path(link: str, source_file: Path) -> str | None:
    """Convert a doc-relative link to its expected site path, or None if external."""
    # Strip anchor/query.
    link = link.split("#", 1)[0].split("?", 1)[0]
    if not link:
        return None
    if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", link):  # http:, mailto:, etc.
        return None
    if link.startswith("//"):
        return None

    if link.startswith("/"):
        target = link
    else:
        # Resolve relative to source file's directory, relative to DOCS_ROOT.
        rel_dir = source_file.parent.relative_to(DOCS_ROOT)
        joined = (rel_dir / link).as_posix()
        # Normalize ../ etc.
        parts: list[str] = []
        for p in joined.split("/"):
            if p in ("", "."):
                continue
            if p == "..":
                if parts:
                    parts.pop()
                continue
            parts.append(p)
        target = "/" + "/".join(parts)

    # Drop trailing slash and common file extensions.
    target = target.rstrip("/")
    target = re.sub(r"\.(mdx?|html)$", "", target)
    if not target:
        target = "/"
    return SITE_PREFIX + target


def build_basename_index() -> dict[str, list[str]]:
    """basename (no extension) -> list of site routes that match."""
    idx: dict[str, list[str]] = defaultdict(list)
    for path in list(DOCS_ROOT.rglob("*.md")) + list(DOCS_ROOT.rglob("*.mdx")):
        rel = path.relative_to(DOCS_ROOT).with_suffix("").as_posix()
        route = "/" + rel
        if route.endswith("/index"):
            route = route[: -len("/index")] or "/"
        idx[path.stem].append(route)
    return idx


def suggest_for_legacy(url: str, basename_idx: dict[str, list[str]]) -> list[str]:
    """Given a legacy docs.lvgl.io URL, return possible local routes by basename."""
    # Strip query/anchor, drop scheme+host, drop version prefix, drop .html.
    path = re.sub(r"[?#].*$", "", url)
    path = LEGACY_HOST.sub("", path)
    path = re.sub(r"\.html?$", "", path)
    leaf = path.rsplit("/", 1)[-1] or path
    return sorted(basename_idx.get(leaf, []))


def main() -> int:
    print(f"Fetching sitemap: {SITEMAP_URL}", file=sys.stderr)
    sitemap = fetch_sitemap_paths()
    print(f"  {len(sitemap)} URLs in sitemap", file=sys.stderr)
    basename_idx = build_basename_index()

    broken: dict[str, list[tuple[Path, int, str, str]]] = defaultdict(list)
    legacy: list[tuple[Path, int, str, str]] = []
    checked_internal = 0
    files = sorted(DOCS_ROOT.rglob("*.md")) + sorted(DOCS_ROOT.rglob("*.mdx"))
    for path in files:
        text = path.read_text(encoding="utf-8", errors="replace")
        for lineno, line in enumerate(text.splitlines(), 1):
            for m in MD_LINK.finditer(line):
                text_part, url = m.group(1), m.group(2)
                if LEGACY_HOST.match(url):
                    legacy.append((path, lineno, text_part, url))
                    continue
                target = to_site_path(url, path)
                if target is None:
                    continue
                checked_internal += 1
                if target not in sitemap:
                    broken[target].append((path, lineno, text_part, url))

    exit_code = 0

    print("\n=== Section 1: internal links missing from sitemap ===")
    if not broken:
        print(f"OK: checked {checked_internal} internal links — none broken.")
    else:
        total_broken = sum(len(v) for v in broken.values())
        print(f"Checked {checked_internal} internal links — {total_broken} broken across {len(broken)} unique targets:\n")
        for target in sorted(broken):
            occurrences = broken[target]
            print(f"BROKEN  {target}  ({len(occurrences)})")
            for path, lineno, text_part, url in occurrences:
                rel = path.relative_to(DOCS_ROOT.parent.parent)
                print(f"    {rel}:{lineno}  [{text_part}]({url})")
            print()
        exit_code = 1

    print("=== Section 2: legacy docs.lvgl.io domain links (need migration to internal routes) ===")
    if not legacy:
        print("OK: no legacy-domain links found.")
    else:
        print(f"Found {len(legacy)} legacy-domain link(s). Suggested local routes are by-basename matches:\n")
        for path, lineno, text_part, url in legacy:
            rel = path.relative_to(DOCS_ROOT.parent.parent)
            suggestions = suggest_for_legacy(url, basename_idx)
            print(f"LEGACY  {rel}:{lineno}")
            print(f"    [{text_part}]({url})")
            if suggestions:
                print(f"    suggested: {', '.join(suggestions)}")
            else:
                print("    suggested: (no basename match — needs manual lookup)")
            print()
        exit_code = 1

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
