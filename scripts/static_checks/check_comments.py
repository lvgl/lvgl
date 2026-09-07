#!/usr/bin/env python3
"""
Reject C++ style (`//`) comments in C source and headers.

LVGL is a C99 library; its `.c` and `.h` files must use block comments
(`/* ... */`) only. C++ files (`.cpp`/`.hpp`) are allowed to use `//`.

The scan uses a small character-level state machine so that `//` sequences
inside string literals (`"http://..."`) and inside block comments
(`/* see https://... */`) are NOT reported -- only genuine `//` comments are.

Usage:
    check_comments.py [path ...]

    With no arguments the default first-party roots are scanned
    (src, demos, examples, tests). Any path argument (file or directory)
    restricts the scan to those paths, e.g.:

        check_comments.py src/core
        check_comments.py src/misc/lv_math.c

Exit codes:
  0 - no `//` comments found
  1 - one or more `//` comments found
"""

from __future__ import annotations

import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Scope
# ---------------------------------------------------------------------------

# Only C sources/headers are checked. C++ translation units may use `//`.
CHECKED_EXTENSIONS = {".c", ".h"}

# Default roots scanned when no path argument is given (mirrors code-format.py).
DEFAULT_ROOTS = ("src", "demos", "examples", "tests")

# Third-party and generated files legitimately contain `//` comments and are
# excluded from formatting in scripts/code-format.cfg. Keep this in sync.
# A path is excluded if any of these fragments appears in its POSIX path.
EXCLUDED_FRAGMENTS = (
    # generated
    "src/lv_conf_internal.h",
    "src/core/lv_obj_style_gen.c",
    "src/core/lv_obj_style_gen.h",
    # third-party libraries
    "src/libs/gif/",
    "src/libs/frogfs/",
    "src/libs/lodepng/",
    "src/libs/qrcode/",
    "src/libs/tjpgd/",
    "src/libs/thorvg/",
    "src/libs/expat/",
    "src/libs/lz4/",
    "src/libs/FT800-FT813/",
    "src/drivers/opengles/glad/",
    "src/others/vg_lite_tvg/vg_lite.h",
    # third-party test/demo assets
    "demos/high_res/fonts/",
    "tests/unity/",
)


def is_excluded(path: Path) -> bool:
    posix = path.as_posix()
    return any(fragment in posix for fragment in EXCLUDED_FRAGMENTS)


# ---------------------------------------------------------------------------
# Detection
# ---------------------------------------------------------------------------

def find_line_comments(text: str) -> list[int]:
    """
    Return the 1-based line numbers that start a `//` comment.

    A character-level state machine ignores `//` that appears inside string
    literals, character literals, and block comments.
    """
    CODE, STRING, CHAR, BLOCK, LINE = range(5)
    state = CODE
    line = 1
    i = 0
    n = len(text)
    hits: list[int] = []

    while i < n:
        c = text[i]
        nxt = text[i + 1] if i + 1 < n else ""

        if c == "\n":
            line += 1
            # A `//` comment runs to end of line.
            if state == LINE:
                state = CODE
            i += 1
            continue

        if state == CODE:
            if c == "/" and nxt == "/":
                hits.append(line)
                state = LINE
                i += 2
                continue
            if c == "/" and nxt == "*":
                state = BLOCK
                i += 2
                continue
            if c == '"':
                state = STRING
                i += 1
                continue
            if c == "'":
                state = CHAR
                i += 1
                continue
            i += 1
            continue

        if state == STRING:
            if c == "\\":
                i += 2  # skip escaped char (e.g. \" )
                continue
            if c == '"':
                state = CODE
            i += 1
            continue

        if state == CHAR:
            if c == "\\":
                i += 2  # skip escaped char (e.g. \' )
                continue
            if c == "'":
                state = CODE
            i += 1
            continue

        if state == BLOCK:
            if c == "*" and nxt == "/":
                state = CODE
                i += 2
                continue
            i += 1
            continue

        # state == LINE: consume until newline (handled above)
        i += 1

    return hits


# ---------------------------------------------------------------------------
# File collection
# ---------------------------------------------------------------------------

def collect_files(roots: list[Path]) -> list[Path]:
    files: list[Path] = []
    for root in roots:
        if root.is_file():
            if root.suffix in CHECKED_EXTENSIONS and not is_excluded(root):
                files.append(root)
            continue
        for ext in CHECKED_EXTENSIONS:
            for path in root.rglob(f"*{ext}"):
                if not is_excluded(path):
                    files.append(path)
    return sorted(set(files))


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(
        description="Reject C++ style (//) comments in C sources and headers."
    )
    parser.add_argument(
        "paths",
        nargs="*",
        help="Files or directories to scan (default: src demos examples tests).",
    )
    parser.add_argument(
        "--root",
        default=".",
        help="Repository root used to resolve default roots (default: cwd).",
    )
    args = parser.parse_args()

    repo_root = Path(args.root).resolve()

    if args.paths:
        roots = [Path(p) for p in args.paths]
    else:
        roots = [repo_root / r for r in DEFAULT_ROOTS]

    roots = [r for r in roots if r.exists()]

    files = collect_files(roots)

    violations: list[tuple[Path, list[int]]] = []
    for path in files:
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        lines = find_line_comments(text)
        if lines:
            violations.append((path, lines))

    if violations:
        total = sum(len(lines) for _, lines in violations)
        print(
            f"Found {total} C++ style (//) comment(s) in "
            f"{len(violations)} file(s). Use block comments (/* ... */):\n"
        )
        for path, lines in violations:
            try:
                shown = path.relative_to(repo_root)
            except ValueError:
                shown = path
            for ln in lines:
                print(f"  {shown}:{ln}")
        sys.exit(1)

    print(f"OK: no // comments in {len(files)} C source/header file(s)")
    sys.exit(0)


if __name__ == "__main__":
    main()
