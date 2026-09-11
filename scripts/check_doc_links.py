#!/usr/bin/env python3
"""Check markdown links in docs/src/.

Built around two concerns with very different CI tolerances:

  ERRORS (always fail CI)
    * Local links — root-relative (/foo) or relative (./foo, ../foo) — that
      do not resolve to a real .mdx file under docs/src/. The local file
      tree is the source of truth; we do not consult the deployed sitemap
      so a new page added in this PR is treated as valid.
    * Raw markdown links into /api or /api-private — these routes are
      auto-generated from C headers (Doxygen -> JSON -> MDX). API
      references must be written with the <ApiLink name="..." /> JSX
      component, not a raw markdown link, per docs/src/contributing/
      writing_docs.mdx.
    * Legacy domain links — anything pointing at docs.lvgl.io/master|latest
      or older versioned mirrors. These are conceptually internal links
      that need to be migrated to current internal routes.

  WARNINGS (never fail CI)
    * Links into /examples — also auto-generated at build time (from the
      examples/ tree at the repo root) so we cannot verify them locally.
      Listed for visibility; treat as "check after deploy".
    * External links (http(s)://...) — probed only when --check-external
      is passed. MANUAL / PERIODIC USE ONLY: ~800 outbound requests per
      run are too slow, flaky, and unfriendly to targets for per-PR CI.
      Schedule weekly or run ad-hoc; never as a required PR gate.

Routing rules (mirrors Astro file-based routing in docs/src/):
  foo.mdx          -> /foo
  foo/index.mdx    -> /foo
  foo/bar.mdx      -> /foo/bar
  docs/src/index.mdx -> /

Convention reminder: this repo writes internal links as /foo/bar (root-
relative). The script accepts ./foo and ../foo too, but they are not the
house style — see docs/src/contributing/writing_docs.mdx.
"""
from __future__ import annotations

import argparse
import re
import sys
import urllib.request
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from collections import defaultdict

DOCS_ROOT = Path(__file__).resolve().parents[1] / "docs" / "src"

# [text](url) — non-greedy text, url stops at whitespace, ) or ".
# Skips images (![...]) by requiring no leading '!' (negative lookbehind).
MD_LINK = re.compile(r"(?<!\!)\[([^\]]*?)\]\(\s*([^\s)]+)(?:\s+\"[^\"]*\")?\s*\)")

# Matches the legacy Sphinx-site domain in any of its forms.
LEGACY_HOST = re.compile(
    r"^https?://(?:docs\.lvgl\.io|lvgl\.io/docs)/(?:master|latest|v\d[^/]*)/",
    re.IGNORECASE,
)

EXTERNAL_OK_STATUSES = {200, 301, 302, 307, 308}
EXTERNAL_TIMEOUT = 10
EXTERNAL_WORKERS = 20
USER_AGENT = "lvgl-doc-link-checker/1.0"

# Auto-generated route prefixes — these exist on the deployed site but have
# no source files under docs/src/, so we can't validate them by walking the
# local tree. Each tuple is (prefix-without-trailing-slash, classification).
#   * /api and /api-private are built from C headers (Doxygen). Authors must
#     use <ApiLink name="..." /> instead of a raw markdown link — error.
#   * /examples is built from the repo's top-level examples/ tree at docs
#     build time, so we cannot verify locally — warning only.
AUTOGEN_PREFIXES: tuple[tuple[str, str], ...] = (
    ("/api", "api"),
    ("/api-private", "api"),
    ("/examples", "example"),
)


def autogen_kind(target: str) -> str | None:
    """Return 'api' / 'example' if `target` is under an auto-generated route, else None."""
    for prefix, kind in AUTOGEN_PREFIXES:
        if target == prefix or target.startswith(prefix + "/"):
            return kind
    return None


def build_local_routes() -> set[str]:
    """Walk docs/src/ and return the set of valid site routes."""
    routes: set[str] = set()
    for path in DOCS_ROOT.rglob("*.mdx"):
        # Skip anything under _-prefixed directories (Astro convention for non-route assets).
        if any(part.startswith("_") for part in path.relative_to(DOCS_ROOT).parts):
            continue
        rel = path.relative_to(DOCS_ROOT).with_suffix("").as_posix()
        route = "/" + rel
        if route.endswith("/index"):
            route = route[: -len("/index")] or "/"
        routes.add(route)
    return routes


def resolve_link(link: str, source_file: Path) -> tuple[str, str] | None:
    """Classify and normalize a link.

    Returns (kind, value) where kind is one of:
      "local"    — value is the normalized site route (e.g. "/integration/rtos/zephyr")
      "api"      — value is the normalized site route under /api or /api-private
      "example"  — value is the normalized site route under /examples
      "legacy"   — value is the original URL (legacy docs.lvgl.io mirror)
      "external" — value is the original URL
    Returns None for things we don't check (anchors, mailto, empty, etc.).
    """
    # Strip anchor/query for resolution purposes; we don't verify anchors.
    bare = link.split("#", 1)[0].split("?", 1)[0]
    if not bare:
        return None  # pure #anchor or empty
    if bare.startswith("//"):
        return ("external", link)
    if LEGACY_HOST.match(link):
        return ("legacy", link)
    if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*:", link):
        # Any scheme — http, https, mailto, ftp, etc.
        if link.lower().startswith(("http://", "https://")):
            return ("external", link)
        return None  # mailto:, tel:, etc. — not link-checkable

    # Local link.
    if bare.startswith("/"):
        target = bare
    else:
        rel_dir = source_file.parent.relative_to(DOCS_ROOT)
        joined = (rel_dir / bare).as_posix()
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

    target = target.rstrip("/")
    target = re.sub(r"\.(mdx?|html)$", "", target)
    if not target:
        target = "/"
    kind = autogen_kind(target)
    if kind is not None:
        return (kind, target)
    return ("local", target)


def check_external(url: str) -> tuple[str, int | None, str | None]:
    """HEAD then GET fallback. Return (url, status_code, error_message)."""
    for method in ("HEAD", "GET"):
        try:
            req = urllib.request.Request(url, method=method, headers={"User-Agent": USER_AGENT})
            with urllib.request.urlopen(req, timeout=EXTERNAL_TIMEOUT) as resp:
                return (url, resp.status, None)
        except urllib.error.HTTPError as e:
            # 4xx/5xx — only retry HEAD->GET on 405 Method Not Allowed.
            if method == "HEAD" and e.code == 405:
                continue
            return (url, e.code, None)
        except Exception as e:
            if method == "HEAD":
                continue
            return (url, None, f"{type(e).__name__}: {e}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "--check-external",
        action="store_true",
        help=(
            "MANUAL / PERIODIC USE ONLY. Also probe external http(s) URLs and warn on "
            "non-2xx/3xx responses. Warnings never affect exit code. Do not enable in "
            "per-PR CI — ~800 outbound requests are slow, flaky, and unfriendly to targets."
        ),
    )
    args = parser.parse_args()

    routes = build_local_routes()
    print(f"Indexed {len(routes)} local routes from {DOCS_ROOT.relative_to(DOCS_ROOT.parents[1])}", file=sys.stderr)

    # (path, lineno, link-text, raw url) for each finding.
    broken_local: dict[str, list[tuple[Path, int, str, str]]] = defaultdict(list)
    api_links: list[tuple[Path, int, str, str, str]] = []   # +resolved target
    legacy: list[tuple[Path, int, str, str]] = []
    example_links: list[tuple[Path, int, str, str, str]] = []  # +resolved target
    externals: list[tuple[Path, int, str, str]] = []

    checked_local = 0
    for path in sorted(DOCS_ROOT.rglob("*.mdx")):
        text = path.read_text(encoding="utf-8", errors="replace")
        for lineno, line in enumerate(text.splitlines(), 1):
            for m in MD_LINK.finditer(line):
                text_part, url = m.group(1), m.group(2)
                resolved = resolve_link(url, path)
                if resolved is None:
                    continue
                kind, value = resolved
                if kind == "api":
                    api_links.append((path, lineno, text_part, url, value))
                elif kind == "example":
                    example_links.append((path, lineno, text_part, url, value))
                elif kind == "legacy":
                    legacy.append((path, lineno, text_part, url))
                elif kind == "external":
                    externals.append((path, lineno, text_part, url))
                elif kind == "local":
                    checked_local += 1
                    if value not in routes:
                        broken_local[value].append((path, lineno, text_part, url))

    exit_code = 0

    # --- Section 1: local links (ERRORS) ---
    print("\n=== Local links (errors) ===")
    if not broken_local:
        print(f"OK: checked {checked_local} local links — all resolve to docs/src/ files.")
    else:
        total = sum(len(v) for v in broken_local.values())
        print(f"ERROR: {total} broken local link(s) across {len(broken_local)} unique targets:\n")
        for target in sorted(broken_local):
            for path, lineno, text_part, url in broken_local[target]:
                rel = path.relative_to(DOCS_ROOT.parents[1])
                print(f"  {rel}:{lineno}")
                print(f"      [{text_part}]({url})  ->  {target}  (no matching file)")
        exit_code = 1

    # --- Section 2: API references via raw markdown link (ERRORS) ---
    print("\n=== API references written as raw markdown links (errors) ===")
    if not api_links:
        print("OK: no markdown links into /api or /api-private.")
    else:
        print(
            f"ERROR: {len(api_links)} markdown link(s) point at /api or /api-private. "
            f"These routes are auto-generated; use the <ApiLink name=\"...\" /> "
            f"component instead (see docs/src/contributing/writing_docs.mdx).\n"
        )
        for path, lineno, text_part, url, target in api_links:
            rel = path.relative_to(DOCS_ROOT.parents[1])
            print(f"  {rel}:{lineno}")
            print(f"      [{text_part}]({url})  ->  {target}")
        exit_code = 1

    # --- Section 3: legacy domain (ERRORS) ---
    print("\n=== Legacy docs.lvgl.io domain links (errors) ===")
    if not legacy:
        print("OK: no legacy-domain links.")
    else:
        print(f"ERROR: {len(legacy)} link(s) still point at docs.lvgl.io/master|latest|...:\n")
        for path, lineno, text_part, url in legacy:
            rel = path.relative_to(DOCS_ROOT.parents[1])
            print(f"  {rel}:{lineno}")
            print(f"      [{text_part}]({url})")
        exit_code = 1

    # --- Section 4: /examples links (WARNINGS) ---
    print("\n=== /examples links — auto-generated routes, cannot verify locally (warnings) ===")
    if not example_links:
        print("OK: no links into /examples.")
    else:
        print(
            f"WARNING: {len(example_links)} link(s) into /examples. These routes are "
            f"built post-CI from the examples/ tree at the repo root, so this script "
            f"cannot verify them. Confirm after deploy:\n"
        )
        for path, lineno, text_part, url, target in example_links:
            rel = path.relative_to(DOCS_ROOT.parents[1])
            print(f"  {rel}:{lineno}")
            print(f"      [{text_part}]({url})  ->  {target}")
        # Warnings do NOT change exit_code.

    # --- Section 5: external links (WARNINGS, opt-in) ---
    print("\n=== External links — MANUAL / PERIODIC CHECK ONLY (warnings, never fails CI) ===")
    print("NOTE: This section is NOT intended for per-PR CI. It issues ~800 outbound")
    print("      HTTP requests; running on every PR is slow, flaky (transient host")
    print("      failures, rate limits, captive portals), and unfriendly to the targets.")
    print("      Run on a schedule (e.g. weekly) or on-demand instead.")
    if not args.check_external:
        print(f"\nSKIPPED: {len(externals)} external link occurrence(s). Pass --check-external to verify.")
    elif not externals:
        print("\nOK: no external links found.")
    else:
        unique = sorted({url for _, _, _, url in externals})
        print(f"\nChecking {len(unique)} unique external URL(s) (HEAD/GET, timeout {EXTERNAL_TIMEOUT}s, {EXTERNAL_WORKERS} workers)...")
        results: dict[str, tuple[int | None, str | None]] = {}
        with ThreadPoolExecutor(max_workers=EXTERNAL_WORKERS) as ex:
            futures = {ex.submit(check_external, u): u for u in unique}
            for fut in as_completed(futures):
                u, status, err = fut.result()
                results[u] = (status, err)
        bad = []
        for path, lineno, text_part, url in externals:
            status, err = results[url]
            if status is not None and status in EXTERNAL_OK_STATUSES:
                continue
            bad.append((path, lineno, text_part, url, status, err))
        if not bad:
            print(f"OK: all {len(unique)} external URL(s) responded with 200/3xx.")
        else:
            print(f"WARNING: {len(bad)} external link occurrence(s) did not return 200/3xx (not failing CI):\n")
            for path, lineno, text_part, url, status, err in bad:
                rel = path.relative_to(DOCS_ROOT.parents[1])
                detail = f"HTTP {status}" if status is not None else err
                print(f"  {rel}:{lineno}  [{detail}]")
                print(f"      [{text_part}]({url})")
            # Warnings do NOT change exit_code.

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
