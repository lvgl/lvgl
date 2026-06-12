#!/usr/bin/env python3
"""
Check that every forward-declared struct/enum in include/lvgl/lv_types.h
has a real definition (with a body) somewhere inside src/.

Rules:
  - Only `typedef struct _Tag` and `typedef enum _Tag` entries are checked.
  - A definition is a line containing `struct _Tag {` or `enum _Tag {`.
  - The `{` must be on the same line as the tag name.
  - lv_types.h itself is never used as evidence of a definition.

Exit codes:
  0 – all forward declarations have a matching definition
  1 – one or more forward declarations are dangling
"""

from __future__ import annotations

import subprocess
import itertools
import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Parsing
# ---------------------------------------------------------------------------

# Matches:
#   typedef struct _lv_obj_t lv_obj_t;
#   typedef enum   _lv_align_t lv_align_t;
FORWARD_DECL_RE = re.compile(
    r"^\s*typedef\s+(struct|enum)\s+(\S+)\s+\S+\s*;",
    re.MULTILINE,
)


def parse_forward_declarations(lv_types_h: Path) -> list[tuple[str, str]]:
    """
    Return [(kind, tag), ...] for every typedef struct/enum in lv_types.h.
    kind is 'struct' or 'enum', tag is the backing name e.g. '_lv_obj_t'.
    """
    content = lv_types_h.read_text(encoding="utf-8", errors="ignore")
    return [
        (m.group(1), m.group(2))
        for m in FORWARD_DECL_RE.finditer(content)
    ]


# ---------------------------------------------------------------------------
# Searching
# ---------------------------------------------------------------------------

SOURCE_EXTENSIONS = {".c", ".cpp", ".h", ".hpp"}


def find_definitions_slow(repo_root: Path, kind: str, tag: str) -> list[Path]:
    """
    Return all files under src_dir that contain a line like:
        struct _lv_obj_t {
        enum   _lv_align_t {
    (whitespace between tag and { is allowed, but both must be on one line)
    """

    src_dir = repo_root / "src"
    include_dir = repo_root / "include" / "lvgl"
    # e.g.  struct\s+_lv_obj_t\s*\{
    pattern = re.compile(
        rf"\b{re.escape(kind)}\s+{re.escape(tag)}\s*\{{"
    )
    found = []

    src_source_files = src_dir.rglob("*")
    include_headers = include_dir.rglob("*")

    for path in sorted(itertools.chain(src_source_files, include_headers)):
        if path.suffix not in SOURCE_EXTENSIONS:
            continue
        try:
            content = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        if pattern.search(content):
            found.append(path)
    return found

def find_definitions(repo_root: Path, kind: str, tag: str) -> list[Path]:
    # Construct the search pattern for grep
    # \b ensures we match word boundaries
    pattern = rf"\b{kind}\s+{tag}\s*\{{"
    
    # Define the search paths
    search_paths = [
        str(repo_root / "src"),
        str(repo_root / "include" / "lvgl")
    ]
    
    # Build the grep command:
    # -r: recursive
    # -l: only print names of files with matches
    # -E: use extended regular expressions
    # --include: filter for specific extensions
    cmd = ["grep", "-rlE", pattern]
    for ext in SOURCE_EXTENSIONS:
        # Converts ".h" to "*.h" for grep
        cmd.append(f"--include=*{ext}")
    
    cmd.extend(search_paths)

    try:
        result = subprocess.run(
            cmd, 
            capture_output=True, 
            text=True, 
            check=False
        )
        
        if result.stdout:
            # Split lines and convert back to Path objects
            return [Path(p) for p in result.stdout.splitlines()]
    except FileNotFoundError:
        # Fallback or error if grep isn't installed (e.g., on Windows)
        print("Grep not found. Falling back to slow search.")
        return find_definitions_slow(repo_root, kind, tag)
        
    return []


# ---------------------------------------------------------------------------
# Main check
# ---------------------------------------------------------------------------

def check_forward_declarations(repo_root: Path) -> bool:
    lv_types_h = repo_root / "include" / "lvgl" / "lv_types.h"

    if not lv_types_h.exists():
        print(f"ERROR: {lv_types_h} not found")
        return False

    declarations = parse_forward_declarations(lv_types_h)

    if not declarations:
        print("WARNING: no typedef struct/enum entries found in lv_types.h")
        return True

    print(f"Found {len(declarations)} forward declarations in lv_types.h")

    dangling: list[tuple[str, str]] = []

    for kind, tag in declarations:
        print(f"{kind} {tag} ->", end= " ")
        hits = find_definitions(repo_root, kind, tag)
        if not hits:
            print(f"KO")
            dangling.append((kind, tag))
        else:
            for h in hits:
                print(f"{h.relative_to(repo_root)} OK")

    if dangling:
        print(
            f"DANGLING forward declarations in lv_types.h "
            f"({len(dangling)} of {len(declarations)}):\n"
        )
        for kind, tag in dangling:
            print(f"  {kind} {tag}")
        return False

    print(
        f"OK: all {len(declarations)} forward declarations in lv_types.h "
        f"have a matching definition"
    )
    return True


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(
        description="Check lv_types.h forward declarations against src/ definitions"
    )
    parser.add_argument(
        "--root",
        default=".",
        help="Repository root (default: current directory)",
    )
    args = parser.parse_args()

    repo_root = Path(args.root).resolve()
    passed = check_forward_declarations(repo_root)
    sys.exit(0 if passed else 1)


if __name__ == "__main__":
    main()
