#!/usr/bin/env python3
"""Clean up generated LVGL example C files.

PURPOSE
-------
Generator output (`screenN_gen.c`) is verbose: section-banner comments, an
empty `style_inited` block, `LV_TRACE_OBJ_CREATE` traces, redundant
`lv_obj_set_name` calls, etc. This script post-processes those files into the
hand-written-looking examples we ship in `examples/`.

INPUT SELECTION
---------------
Walks `examples/` recursively and operates only on `.c` files that have a
sibling `.xml` of the same basename. The `.xml` is both the gating signal
(only generator-produced examples are touched) and a source of metadata
(per-element comments + a top-level description).

PIPELINE
--------
Each file is run through `TRANSFORMATIONS` in order. Every transformation is
`(source: str, path: Path) -> str` and pure (no side effects); the runner
writes the file back only if the final text differs from the original.

Transformations are grouped at the bottom of the file:

  * Generator boilerplate removal — strip `style_inited`, `LV_TRACE_*`,
    `lv_obj_set_name(_static)`, section banner comments, `@brief` line in the
    file header.
  * Code modernization — collapse adjacent width/height into `set_size` and
    x/y into `set_pos`; replace `lv_obj_create(NULL)` root with
    `lv_screen_active()`; drop the trailing `return screen;`.
  * Identity / naming — rename `screenN_create` to `<file_stem>_create`,
    update `@file`, collapse all `#include`s to a single relative
    `lvgl.h` include.
  * XML doc mapping — top-level `<!-- ... -->` becomes a doxygen block
    above the function (with `@title`/`@brief` recognised on re-runs);
    element-preceding XML comments become `/* */` comments above the
    matching `lv_<type>_create(...)` line.
  * Whitespace cleanup — remove blank lines right after `{`.

IDEMPOTENCY
-----------
All transformations are designed to be safe to re-run: regex anchors and
"already in target state?" checks let you run the script against output
that's already been processed (e.g. after editing an `.xml` and re-running)
without duplicating content.
"""

from __future__ import annotations

import os
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Repo root = parent of `scripts/`. Used to (a) locate the `examples/` tree
# and (b) compute the relative path to `lvgl.h` for include rewriting.
REPO_ROOT = Path(__file__).resolve().parent.parent


# =============================================================================
# Generator boilerplate removal
# =============================================================================

# Matches the empty style-init scaffold the generator always emits:
#
#     static bool style_inited = false;
#
#     if (!style_inited) {
#         <only whitespace>
#         style_inited = true;
#     }
#
# When a real style-init block has body content we leave it alone; the inner
# `(?:[ \t]*\n)*` only consumes blank lines between brace and `style_inited =
# true;`. The trailing `(?:[ \t]*\n)?` eats one blank line below so we don't
# leave a yawning gap after removal.
EMPTY_STYLE_INITED_RE = re.compile(
    r"""
    [ \t]*static\ bool\ style_inited\ =\ false;[ \t]*\n
    (?:[ \t]*\n)*
    [ \t]*if\ \(!style_inited\)\ \{[ \t]*\n
    (?:[ \t]*\n)*
    [ \t]*style_inited\ =\ true;[ \t]*\n
    [ \t]*\}[ \t]*\n
    (?:[ \t]*\n)?
    """,
    re.VERBOSE,
)


def remove_empty_style_inited(source: str) -> str:
    return EMPTY_STYLE_INITED_RE.sub("", source)


# Trace calls like `LV_TRACE_OBJ_CREATE("begin");` — pure debug noise in the
# generated files. Optional trailing blank line is consumed too.
LV_TRACE_OBJ_CREATE_RE = re.compile(
    r"[ \t]*LV_TRACE_OBJ_CREATE\([^)]*\);[ \t]*\n(?:[ \t]*\n)?"
)


def remove_lv_trace_obj_create(source: str) -> str:
    return LV_TRACE_OBJ_CREATE_RE.sub("", source)


# The generator emits `lv_obj_set_name(obj, "...")` (and `_static` variant)
# for debugging-friendly object naming. Hand-written examples don't carry it.
LV_OBJ_SET_NAME_RE = re.compile(
    r"[ \t]*lv_obj_set_name(?:_static)?\([^)]*\);[ \t]*\n"
)


def remove_lv_obj_set_name(source: str) -> str:
    return LV_OBJ_SET_NAME_RE.sub("", source)


# Three-line section banner comments such as:
#
#     /*********************
#      *      INCLUDES
#      *********************/
#
# The opening sequence requires 3+ asterisks (`/\*{3,}`), which deliberately
# excludes doxygen file headers (`/**`).
SECTION_BANNER_RE = re.compile(
    r"[ \t]*/\*{3,}[ \t]*\n"
    r"[ \t]*\*[^\n]*\n"
    r"[ \t]*\*{3,}/[ \t]*\n"
    r"(?:[ \t]*\n)?"
)


def remove_section_banners(source: str) -> str:
    return SECTION_BANNER_RE.sub("", source)


# Drop the ` * @brief ...` line inside the generator's file-header doxygen
# block. The file directive (`@file`) is updated later by `fix_file_directive`.
BRIEF_LINE_RE = re.compile(r"[ \t]*\*[ \t]*@brief[^\n]*\n")


def remove_brief_line(source: str) -> str:
    return BRIEF_LINE_RE.sub("", source)


# =============================================================================
# Code modernization
# =============================================================================

# Replace the generator's `lv_obj_create(NULL)` root with `lv_screen_active()`
# and rename all references to that root (default name `lv_obj_0`) to `screen`.
ROOT_OBJ_CREATE_RE = re.compile(
    r"lv_obj_t\s*\*\s*lv_obj_0\s*=\s*lv_obj_create\s*\(\s*NULL\s*\)\s*;"
)
LV_OBJ_0_REF_RE = re.compile(r"\blv_obj_0\b")


def root_obj_to_screen(source: str) -> str:
    source = ROOT_OBJ_CREATE_RE.sub("lv_obj_t * screen = lv_screen_active();", source)
    return LV_OBJ_0_REF_RE.sub("screen", source)


# Argument-content pattern that tolerates one level of nested parens, so we
# can match values like `lv_pct(100)` without truncating at the inner `)`.
# Without this, `[^)]+` would stop at the first `)`.
_ARG_PAT = r"(?:[^()]|\([^)]*\))*"

# Adjacent width/height pair on the same object → `lv_obj_set_size(obj, W, H)`.
# Group 1 = indentation, group 2 = object name (must match on both lines via
# the backreference `\2`), groups 3/4 = the width/height values.
WIDTH_HEIGHT_PAIR_RE = re.compile(
    rf"([ \t]*)lv_obj_set_width\(\s*([A-Za-z_]\w*)\s*,\s*({_ARG_PAT})\)\s*;[ \t]*\n"
    rf"\1lv_obj_set_height\(\s*\2\s*,\s*({_ARG_PAT})\)\s*;"
)

# Same shape, for x/y → `lv_obj_set_pos`. Note: we don't handle the reverse
# order (height-then-width or y-then-x) because the generator's output is
# consistent. Add a sibling regex if that assumption breaks.
X_Y_PAIR_RE = re.compile(
    rf"([ \t]*)lv_obj_set_x\(\s*([A-Za-z_]\w*)\s*,\s*({_ARG_PAT})\)\s*;[ \t]*\n"
    rf"\1lv_obj_set_y\(\s*\2\s*,\s*({_ARG_PAT})\)\s*;"
)


def combine_size_and_pos(source: str) -> str:
    source = WIDTH_HEIGHT_PAIR_RE.sub(r"\1lv_obj_set_size(\2, \3, \4);", source)
    source = X_Y_PAIR_RE.sub(r"\1lv_obj_set_pos(\2, \3, \4);", source)
    return source


# Drop the trailing `return screen;` — the root is now `lv_screen_active()`,
# the function is `void`, so returning is meaningless. We consume one leading
# blank line so the function body doesn't end on dead whitespace.
RETURN_SCREEN_RE = re.compile(r"(?:[ \t]*\n)?[ \t]*return\s+screen\s*;[ \t]*\n")


def remove_return_screen(source: str, path: Path) -> str:
    return RETURN_SCREEN_RE.sub("", source)


# =============================================================================
# Identity / naming
# =============================================================================

# `lv_obj_t * <name>_create(void)` → `void <file_stem>_create(void)`.
# The generator names the entry function either by screen index
# (`screenN_create`, older builds) or by the screen XML's filename
# (`<file_stem>_create`, current builds). The broader `\w+_create` match
# handles both. The new return type is `void` because `remove_return_screen`
# already dropped the return statement — leaving `lv_obj_t *` would emit a
# function declared to return a pointer with no return statement.
CREATE_FUNC_DECL_RE = re.compile(
    r"lv_obj_t\s*\*\s*\w+_create\s*\(\s*void\s*\)"
)


def rename_create_function(source: str, path: Path) -> str:
    return CREATE_FUNC_DECL_RE.sub(f"void {path.stem}_create(void)", source)


# Collapse the contiguous `#include` block at the top of the file into a
# single relative include of `lvgl.h`. `count=1` ensures we only touch the
# first such block, leaving any deeper `#include` (rare) untouched.
INCLUDE_BLOCK_RE = re.compile(
    r"(?:#include[ \t]+[\"<][^\">\n]+[\">][ \t]*\n)+"
)


def replace_includes_with_lvgl(source: str, path: Path) -> str:
    # Relative path is recomputed per file so this works for any directory depth.
    rel = os.path.relpath(REPO_ROOT / "lvgl.h", path.parent)
    return INCLUDE_BLOCK_RE.sub(f'#include "{rel}"\n', source, count=1)


# Update the ` * @file <whatever>` line inside the existing file-header
# doxygen block to use the actual filename. Does nothing if no `@file` exists.
FILE_DIRECTIVE_RE = re.compile(r"(\*[ \t]*@file)[ \t]+[^\n]*")


def fix_file_directive(source: str, path: Path) -> str:
    return FILE_DIRECTIVE_RE.sub(lambda m: f"{m.group(1)} {path.name}", source)


# =============================================================================
# XML doc mapping
# =============================================================================
#
# The sibling `.xml` carries semantic comments that the generator drops:
#
#   * A top-level `<!-- ... -->` block (typically with `@title`/`@brief`)
#     before `<screen>` — becomes a doxygen `/** ... */` block above the
#     create function.
#   * Inline `<!-- ... -->` before each element under `<view>` — becomes a
#     `/* ... */` comment above the matching `lv_<type>_create(...)` line in
#     the C file.

# Match a `lv_obj_t * <name> = lv_<type>_create(...)` line so we can map
# XML annotations to the right C creation by tag/order. Group 1 = indent,
# group 2 = the `lv_<type>` token (e.g. `lv_arc`, `lv_label`).
CREATE_LINE_RE = re.compile(
    r"([ \t]*)lv_obj_t\s*\*\s*\w+\s*=\s*(lv_\w+)_create\s*\("
)


def _xml_elements_with_comments(xml_path: Path) -> list[tuple[str, str | None]]:
    """Return (tag, preceding_comment) for each XML element in document order.

    A comment binds to the next element that follows it, regardless of
    nesting depth (a comment inside `<view>` followed by `<lv_arc>` annotates
    that arc; a comment inside `<lv_arc>` followed by `<lv_label>` annotates
    that label).

    Top-level comments outside the root (e.g. before `<screen>`) are NOT
    returned here — those are handled separately by
    `add_top_level_doc_comment` via a regex on the raw text, because
    `ET.parse` discards everything outside the root.
    """
    parser = ET.XMLParser(target=ET.TreeBuilder(insert_comments=True))
    tree = ET.parse(xml_path, parser=parser)
    root = tree.getroot()

    annotations: list[tuple[str, str | None]] = []
    pending: str | None = None

    def walk(elem):
        nonlocal pending
        for child in elem:
            if child.tag is ET.Comment:
                # Comments get stored, awaiting the next non-comment element.
                pending = (child.text or "").strip()
            else:
                annotations.append((child.tag, pending))
                pending = None
                # Recurse so nested elements are emitted in document order.
                walk(child)

    walk(root)
    return annotations


# A comment block at the very start of the XML file (before any element).
# Used as the source for the per-function doxygen block.
TOP_LEVEL_XML_COMMENT_RE = re.compile(r"\A\s*<!--(.*?)-->", re.DOTALL)

# Locates the `void <name>_create(void)` line so we can insert the doxygen
# block immediately above it.
FUNCTION_DECL_RE = re.compile(
    r"^void\s+\w+_create\s*\(\s*void\s*\)", re.MULTILINE
)

# Matches a single `/** ... */` block at the very end of a substring (we slice
# `source[:fn_start]` and search this against it). Important nuance: the inner
# pattern `(?:[^*]|\*(?!/))*` refuses to cross a closing `*/`. Without that
# guard, the more obvious `[\s\S]*?\*/` combined with the trailing `\Z`
# anchor will silently expand across multiple adjacent doc blocks (because
# non-greedy backtracks until the `\Z` anchor is satisfied) — which used to
# eat the `@file` header along with the `@title` block above the function.
EXISTING_DOC_BLOCK_RE = re.compile(
    r"/\*\*(?:[^*]|\*(?!/))*\*/[ \t]*\n\s*\Z"
)


def _format_doxygen_block(body: str) -> str:
    """Wrap a multi-line plain-text body into `/** * ... */` form.

    Blank lines become bare ` *` (so doxygen renders them as paragraph
    breaks); non-blank lines are reindented under ` * `.
    """
    out = ["/**"]
    for line in body.splitlines():
        stripped = line.strip()
        out.append(f" * {stripped}" if stripped else " *")
    out.append(" */")
    return "\n".join(out)


def add_top_level_doc_comment(source: str, path: Path) -> str:
    """Lift the XML's top-level comment into a doxygen block above the function.

    Idempotent: if a `@title`/`@brief`-bearing block already sits right above
    the function, it gets replaced rather than stacked. The (separate) `@file`
    header at the top of the file is preserved because `EXISTING_DOC_BLOCK_RE`
    can't span across multiple doc blocks (see the note on that regex).
    """
    xml_path = path.with_suffix(".xml")
    if not xml_path.exists():
        return source

    m = TOP_LEVEL_XML_COMMENT_RE.match(xml_path.read_text())
    if not m or not m.group(1).strip():
        return source

    comment_block = _format_doxygen_block(m.group(1).strip()) + "\n"

    fn_match = FUNCTION_DECL_RE.search(source)
    if not fn_match:
        return source
    fn_start = fn_match.start()

    # If a pre-existing @title/@brief doc block sits right above the function,
    # strip it before inserting the fresh one (so re-runs replace, not stack).
    # The "@title"/"@brief" content check ensures we only touch *our* block —
    # not, say, a legit doxygen comment the user wrote about the function.
    before_fn = source[:fn_start]
    existing = EXISTING_DOC_BLOCK_RE.search(before_fn)
    if existing and ("@title" in existing.group(0) or "@brief" in existing.group(0)):
        source = source[: existing.start()] + source[fn_start:]
        fn_start = existing.start()

    return source[:fn_start] + comment_block + source[fn_start:]


def _format_inline_comment(comment: str, indent: str) -> list[str]:
    """Format an XML comment as 1+ properly indented C block-comment lines.

    The XML source preserves whatever whitespace sat between `<!--` and `-->`
    (typically tabs from nested XML indentation). Emitting `/* {comment} */`
    verbatim leaves continuation lines stuck with XML indentation, which
    looks broken in C source. We strip each line and re-indent continuations
    under the start of the first comment word (3 spaces past `indent` to
    clear the opening `/* `).
    """
    lines = [ln.strip() for ln in comment.splitlines()]
    # Drop blank leading/trailing lines so re-indent doesn't create stray
    # empty comment rows.
    while lines and not lines[0]:
        lines.pop(0)
    while lines and not lines[-1]:
        lines.pop()
    if not lines:
        return []
    if len(lines) == 1:
        return [f"{indent}/* {lines[0]} */\n"]
    cont = indent + "   "
    out = [f"{indent}/* {lines[0]}\n"]
    for ln in lines[1:-1]:
        out.append(f"{cont}{ln}\n")
    out.append(f"{cont}{lines[-1]} */\n")
    return out


def _strip_preceding_comment(out_lines: list[str]) -> None:
    """Pop any single-line `//` or `/* */` block comment at the tail of out_lines.

    Used by `map_xml_comments` for idempotency: re-running the cleanup
    should replace a stale comment block (e.g. an earlier misindented copy,
    or the legacy `// ...` form), not stack a new one on top of it.
    """
    if not out_lines:
        return
    last = out_lines[-1].rstrip()
    if last.lstrip().startswith("//"):
        out_lines.pop()
        return
    if last.endswith("*/"):
        # Walk back to the line that opens the block; both single-line and
        # multi-line `/* */` shapes collapse to this.
        i = len(out_lines) - 1
        while i >= 0 and "/*" not in out_lines[i]:
            i -= 1
        if i >= 0:
            del out_lines[i:]


def map_xml_comments(source: str, path: Path) -> str:
    """Map per-element XML comments to `/* */` comments above C creations.

    Pairing is by sequential order: the Nth `lv_<type>_create(...)` line in
    the C file is associated with the Nth `lv_*` element in the XML's
    document order (after filtering out wrapper tags like `<view>`).

    The tag is verified to match (e.g. an `lv_arc` create line only consumes
    an `lv_arc` annotation) — a mismatch likely means the C file diverged
    from the XML, in which case we'd want to bail rather than misattribute.
    Currently we silently skip mismatches; tighten this if the script starts
    inserting comments in wrong places.

    Idempotent across comment styles and shapes: any existing `//` or
    `/* */` block immediately above the create line is stripped and replaced
    so re-runs converge instead of accumulating. (Generated files don't have
    hand-written comments above create calls — every such comment is
    something this function previously inserted.)
    """
    xml_path = path.with_suffix(".xml")
    if not xml_path.exists():
        return source

    # Drop wrapper tags (e.g. `<view>`) that have no C counterpart — without
    # this filter the pairing-by-index gets shifted by one.
    annotations = [
        (tag, comment)
        for tag, comment in _xml_elements_with_comments(xml_path)
        if tag.startswith("lv_")
    ]

    out_lines: list[str] = []
    idx = 0
    for line in source.splitlines(keepends=True):
        m = CREATE_LINE_RE.match(line)
        if m and idx < len(annotations):
            tag, comment = annotations[idx]
            if tag == m.group(2) and comment:
                indent = m.group(1)
                _strip_preceding_comment(out_lines)
                out_lines.extend(_format_inline_comment(comment, indent))
            # Always advance the annotation pointer for any create line so
            # one missing comment doesn't desync the whole mapping.
            idx += 1
        out_lines.append(line)

    return "".join(out_lines)


# =============================================================================
# Whitespace cleanup
# =============================================================================

# Strip blank lines that sit directly after an opening `{`. After earlier
# transformations remove `LV_TRACE_OBJ_CREATE("begin");` and the
# `style_inited` scaffold, the function body often starts with a stray blank
# line — this cleans that up.
BLANK_AFTER_OPEN_BRACE_RE = re.compile(r"(\{[ \t]*\n)(?:[ \t]*\n)+")


def remove_blank_after_open_brace(source: str) -> str:
    return BLANK_AFTER_OPEN_BRACE_RE.sub(r"\1", source)


# Strip trailing spaces/tabs on every line. The generator occasionally emits
# `    ` (a "blank" line that's actually 4 spaces) between blocks, which is
# both unsightly and tends to upset code-format tools.
TRAILING_WS_RE = re.compile(r"[ \t]+$", re.MULTILINE)


def strip_trailing_whitespace(source: str) -> str:
    return TRAILING_WS_RE.sub("", source)


# =============================================================================
# Pipeline
# =============================================================================
#
# Every transformation has signature `(source: str, path: Path) -> str`.
# Functions that don't need `path` are wrapped in a `lambda` so the runner
# can call them uniformly. The order matters in a few places:
#
#   * `remove_return_screen` and `rename_create_function` must both run
#     before any reader assumes the function is `void` — together they make
#     it so.
#   * `replace_includes_with_lvgl` must run before `fix_file_directive`
#     would be meaningless either way, but conceptually the include block
#     belongs to the "file identity" group.
#   * `add_top_level_doc_comment` reads from the XML and writes above the
#     function, so it must run after `rename_create_function` (so the
#     function decl is already in its final shape).
#   * `remove_blank_after_open_brace` runs last so it can clean up blanks
#     left by everything else.
TRANSFORMATIONS = [
    # Generator boilerplate.
    lambda s, p: remove_empty_style_inited(s),
    lambda s, p: remove_lv_trace_obj_create(s),
    lambda s, p: remove_lv_obj_set_name(s),
    # Code modernization.
    lambda s, p: root_obj_to_screen(s),
    lambda s, p: combine_size_and_pos(s),
    remove_return_screen,
    # Identity / naming.
    rename_create_function,
    lambda s, p: remove_section_banners(s),
    lambda s, p: remove_brief_line(s),
    replace_includes_with_lvgl,
    fix_file_directive,
    # XML doc mapping.
    add_top_level_doc_comment,
    map_xml_comments,
    # Whitespace cleanup (always last so it can mop up).
    lambda s, p: remove_blank_after_open_brace(s),
    lambda s, p: strip_trailing_whitespace(s),
]


# =============================================================================
# Runner
# =============================================================================


def find_target_files(examples_dir: Path) -> list[Path]:
    """Yield every `.c` under `examples_dir` that has a sibling `.xml`."""
    targets = []
    for c_file in examples_dir.rglob("*.c"):
        if c_file.with_suffix(".xml").exists():
            targets.append(c_file)
    return sorted(targets)


def process(path: Path) -> bool:
    """Run all transformations on a file. Return True if it changed on disk."""
    original = path.read_text()
    updated = original
    for fn in TRANSFORMATIONS:
        updated = fn(updated, path)
    if updated != original:
        path.write_text(updated)
        return True
    return False


def main(argv: list[str]) -> int:
    examples_dir = REPO_ROOT / "examples"
    if not examples_dir.is_dir():
        print(f"examples directory not found: {examples_dir}", file=sys.stderr)
        return 1

    targets = find_target_files(examples_dir)
    if not targets:
        print("No .c files with matching .xml siblings found.")
        return 0

    changed = 0
    for path in targets:
        if process(path):
            changed += 1
            print(f"updated: {path.relative_to(REPO_ROOT)}")

    print(f"\n{changed} of {len(targets)} files changed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
