#!/usr/bin/env python3
"""Clean up generated LVGL example C files.

Generator output (`screenN_gen.c`) is verbose — section-banner comments, an
empty `style_inited` block, `LV_TRACE_OBJ_CREATE` traces, redundant
`lv_obj_set_name` calls, etc. — and this script post-processes those files
into the hand-written-looking examples we ship in `examples/`.

Each `.c` file that has a sibling `.xml` of the same basename is run through
`TRANSFORMATIONS` in order; each transform is `(source, path) -> str` and
pure, so the runner only writes back when the final text differs. Every
transform is idempotent — safe to re-run on already-processed output without
duplicating content.
"""

from __future__ import annotations

import os
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Repo root = parent of `scripts/`. Used to (a) locate the `examples/` tree
# and (b) compute the relative path to `examples/lv_examples.h` for include
# rewriting.
REPO_ROOT = Path(__file__).resolve().parent.parent

# Subjects declared at the project level (one source of truth) so each example
# can pull the names/types/initial-values it actually references.
GLOBALS_XML_PATH = REPO_ROOT / "examples" / "xml_project" / "globals.xml"

# Fixed buffer size used for string subjects when promoted into example-local
# inits. The project's gen header uses `UI_SUBJECT_STRING_LENGTH = 256`; the
# example files don't include that header so we inline the literal here.
SUBJECT_STRING_BUF_SIZE = 256


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

# `lv_obj_t * <name>_create(void)` → `void <file_stem>(void)`.
# The generator names the entry function either by screen index
# (`screenN_create`, older builds) or by the screen XML's filename
# (`<file_stem>_create`, current builds). The broader `\w+_create` match
# handles both. We drop the `_create` suffix entirely so example functions
# read as `void lv_example_<feature>(void)` — matching the legacy C-only
# examples. The new return type is `void` because `remove_return_screen`
# already dropped the return statement — leaving `lv_obj_t *` would emit a
# function declared to return a pointer with no return statement.
CREATE_FUNC_DECL_RE = re.compile(
    r"lv_obj_t\s*\*\s*\w+_create\s*\(\s*void\s*\)"
)


def rename_create_function(source: str, path: Path) -> str:
    return CREATE_FUNC_DECL_RE.sub(f"void {path.stem}(void)", source)


# Collapse the contiguous `#include` block at the top of the file into a
# single relative include of `examples/lv_examples.h` (the shared example
# header that pulls in `lvgl.h` plus the per-topic prototypes). `count=1`
# ensures we only touch the first such block, leaving any deeper `#include`
# (rare) untouched.
INCLUDE_BLOCK_RE = re.compile(
    r"(?:#include[ \t]+[\"<][^\">\n]+[\">][ \t]*\n)+"
)


def replace_includes_with_lv_examples(source: str, path: Path) -> str:
    # Relative path is recomputed per file so this works for any directory depth.
    rel = os.path.relpath(REPO_ROOT / "examples" / "lv_examples.h", path.parent)
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
    r"^void\s+lv_example_\w+\s*\(\s*void\s*\)", re.MULTILINE
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
# Local subject inits
# =============================================================================
#
# Subjects are declared once in `examples/xml_project/globals.xml`. The
# generator emits `&subject_X` references but assumes those subjects are
# externs defined by project init code. For standalone example files we
# inject the decl + init for each referenced subject so a reader can
# copy-paste the example and have it work.
#
# Note the label `style_inited` survives this section because
# `remove_empty_style_inited` downstream matches that literal name; the
# rename to `inited` happens after empty-block removal.


def _load_globals_subjects() -> dict[str, dict]:
    """Return a mapping `name -> {type, value, min_value, max_value}` from
    `globals.xml`.

    Cached on first call. Subjects keep their declaration order (Python 3.7+
    dict preserves insertion order), which we lean on when emitting init
    blocks so the example files stay diff-stable run-to-run.
    """
    cache = getattr(_load_globals_subjects, "_cache", None)
    if cache is not None:
        return cache

    subjects: dict[str, dict] = {}
    if GLOBALS_XML_PATH.exists():
        tree = ET.parse(GLOBALS_XML_PATH)
        subjects_elem = tree.getroot().find("subjects")
        if subjects_elem is not None:
            for child in subjects_elem:
                # ElementTree gives us comment nodes too when comments are
                # captured; filter those out by checking the tag is a real
                # string.
                if not isinstance(child.tag, str):
                    continue
                name = child.get("name")
                if not name:
                    continue
                subjects[name] = {
                    "type": child.tag,  # "int", "string", "float"
                    "value": child.get("value", "0"),
                    "min_value": child.get("min_value"),
                    "max_value": child.get("max_value"),
                }

    _load_globals_subjects._cache = subjects  # type: ignore[attr-defined]
    return subjects


def _subject_decl_lines(name: str, meta: dict) -> list[str]:
    """Top-of-function static declarations for a subject (4-space indent).

    `string` subjects need two backing buffers — value and previous-value —
    plus the subject itself.
    """
    out = [f"    static lv_subject_t {name};"]
    if meta["type"] == "string":
        out.append(f"    static char {name}_buf[{SUBJECT_STRING_BUF_SIZE}];")
        out.append(f"    static char {name}_prev_buf[{SUBJECT_STRING_BUF_SIZE}];")
    return out


def _subject_init_lines(name: str, meta: dict) -> list[str]:
    """Init-block calls for a subject (8-space indent, inside `if (!inited)`)."""
    typ = meta["type"]
    value = meta["value"]
    out: list[str] = []
    if typ == "int":
        out.append(f"        lv_subject_init_int(&{name}, {value});")
        if meta.get("min_value") is not None:
            out.append(
                f"        lv_subject_set_min_value_int(&{name}, {meta['min_value']});"
            )
        if meta.get("max_value") is not None:
            out.append(
                f"        lv_subject_set_max_value_int(&{name}, {meta['max_value']});"
            )
    elif typ == "float":
        out.append(f"        lv_subject_init_float(&{name}, {value});")
    elif typ == "string":
        # Multi-line call to match the canonical formatting of generated
        # project code; the args are too long to fit comfortably on one line.
        out.extend([
            f"        lv_subject_init_string(&{name},",
            f"                               {name}_buf,",
            f"                               {name}_prev_buf,",
            f"                               {SUBJECT_STRING_BUF_SIZE},",
            f'                               "{value}");',
        ])
    return out


# Locates the function body's opening brace. We pair it with FUNCTION_DECL_RE
# (defined earlier) when we need to insert a fresh `style_inited` block in a
# file that didn't have one.
FUNCTION_OPEN_BRACE_RE = re.compile(
    r"^(void\s+lv_example_\w+\s*\(\s*void\s*\))\s*\n\{\s*\n", re.MULTILINE
)

# Captures the body of the existing `static bool style_inited` block so we
# can splice subject inits in just before the `style_inited = true;` line.
INITED_BLOCK_RE = re.compile(
    r"""
    (?P<head>[ \t]*static\ bool\ style_inited\ =\ false;[ \t]*\n
             (?:[ \t]*\n)*
             [ \t]*if\ \(!style_inited\)\ \{[ \t]*\n)
    (?P<body>(?:.*?\n)*?)
    (?P<tail>[ \t]*style_inited\ =\ true;[ \t]*\n
             [ \t]*\}[ \t]*\n)
    """,
    re.VERBOSE,
)


def _used_subjects(source: str) -> list[str]:
    """Return subjects from globals.xml referenced as `&name` in `source`,
    keeping `globals.xml` order (for stable output)."""
    meta = _load_globals_subjects()
    return [n for n in meta if re.search(rf"&{re.escape(n)}\b", source)]


def init_subjects(source: str, path: Path) -> str:
    """Add `static lv_subject_t` declarations + init calls for any subject
    the example references, so the file is self-contained.

    Idempotent: a subject that already has a `static lv_subject_t <name>`
    declaration isn't re-declared, and one that already has an
    `lv_subject_init_*(&<name>` call isn't re-initialised.
    """
    meta = _load_globals_subjects()
    if not meta:
        return source

    used = _used_subjects(source)
    if not used:
        return source

    new_decls: list[str] = []
    new_inits: list[str] = []
    for name in used:
        if not re.search(rf"\bstatic\s+lv_subject_t\s+{re.escape(name)}\b", source):
            new_decls.extend(_subject_decl_lines(name, meta[name]))
        if not re.search(
            rf"lv_subject_init_(?:int|float|string)\s*\(\s*&{re.escape(name)}\b",
            source,
        ):
            new_inits.extend(_subject_init_lines(name, meta[name]))

    if not new_decls and not new_inits:
        return source

    block_match = INITED_BLOCK_RE.search(source)
    if block_match:
        # The example already has a `style_inited` block (typically because
        # it has styles). Splice the subject decls in just before the block
        # and the init lines in just before `style_inited = true;`.
        block_start = block_match.start()
        head = block_match.group("head")
        body = block_match.group("body")
        tail = block_match.group("tail")

        if new_inits:
            extra = "\n".join(new_inits) + "\n"
            body = body + extra

        new_block = head + body + tail

        before_block = source[:block_start]
        after_block = source[block_match.end():]

        if new_decls:
            # Insert decls just before the block, separated by a blank line
            # from any earlier static declarations.
            decl_text = "\n".join(new_decls) + "\n\n"
            before_block = before_block + decl_text

        return before_block + new_block + after_block

    # No existing block — synthesise one right after the function's `{`.
    fn_match = FUNCTION_OPEN_BRACE_RE.search(source)
    if not fn_match:
        return source

    insert_pos = fn_match.end()
    lines: list[str] = []
    if new_decls:
        lines.extend(new_decls)
        lines.append("")
    lines.append("    static bool style_inited = false;")
    lines.append("")
    lines.append("    if (!style_inited) {")
    lines.extend(new_inits)
    lines.append("        style_inited = true;")
    lines.append("    }")
    lines.append("")
    block_text = "\n".join(lines) + "\n"
    return source[:insert_pos] + block_text + source[insert_pos:]


# =============================================================================
# Image declarations
# =============================================================================
#
# The generator emits `lv_image_set_src(img, my_image)` for registered image
# names. To compile against a C-array image the call needs to pass `&my_image`
# (an `lv_image_dsc_t *`) and the example must declare the symbol via
# `LV_IMAGE_DECLARE(my_image)` so the linker can resolve the extern.
#
# Image names that look like LVGL constants (start with `LV_`) — e.g.
# `LV_SYMBOL_OK` — are skipped because those are string macros, not C-array
# image descriptors.

# Captures the `lv_image_set_src(obj, [&]name)` call. Groups:
#   1 = call prefix up to and including the `,`/whitespace before the arg
#   2 = optional `&` already present (idempotency)
#   3 = the image identifier
LV_IMAGE_SET_SRC_RE = re.compile(
    r"(lv_image_set_src\s*\(\s*[A-Za-z_]\w*\s*,\s*)(&?)([A-Za-z_]\w*)"
)

# A registered image can also reach a style by name through the image-src
# style setters — `lv_style_set_bg_image_src(&style, name)`,
# `lv_style_set_arc_image_src(...)`, `lv_obj_set_style_bg_image_src(obj,
# name, sel)`, … — which need the same `&name` + `LV_IMAGE_DECLARE` fix. The
# first argument is a `&style`/`obj`, so it is matched loosely as `[^,]+`.
LV_STYLE_IMAGE_SRC_RE = re.compile(
    r"(lv_(?:style_set|obj_set_style)_\w*image_src\s*\(\s*[^,]+,\s*)(&?)([A-Za-z_]\w*)"
)


def declare_and_ref_images(source: str, path: Path) -> str:
    images: list[str] = []  # preserve first-seen order for stable output

    def repl(m: re.Match) -> str:
        prefix, amp, name = m.group(1), m.group(2), m.group(3)
        if name.startswith("LV_"):
            # `LV_SYMBOL_*` etc. — symbol macros, not C-array images.
            return m.group(0)
        if name not in images:
            images.append(name)
        return f"{prefix}&{name}" if not amp else m.group(0)

    new_source = LV_IMAGE_SET_SRC_RE.sub(repl, source)
    new_source = LV_STYLE_IMAGE_SRC_RE.sub(repl, new_source)
    if not images:
        return new_source

    # Add `LV_IMAGE_DECLARE(name);` once per image at the top of the function
    # body. Idempotent: skip names that already have a declare line.
    fn_match = FUNCTION_OPEN_BRACE_RE.search(new_source)
    if not fn_match:
        return new_source

    insert_pos = fn_match.end()
    decls: list[str] = []
    for img in images:
        if not re.search(rf"LV_IMAGE_DECLARE\s*\(\s*{re.escape(img)}\s*\)", new_source):
            decls.append(f"    LV_IMAGE_DECLARE({img});")
    if not decls:
        return new_source

    block = "\n".join(decls) + "\n\n"
    return new_source[:insert_pos] + block + new_source[insert_pos:]


# =============================================================================
# Image declarations from globals.xml
# =============================================================================
#
# `declare_and_ref_images` above only covers images that appear as the direct
# argument to `lv_image_set_src`. This companion transform reads the image
# registry from `globals.xml` and inserts `LV_IMAGE_DECLARE(name)` for every
# registered image whose name appears anywhere in the source as a bare C
# identifier — covering setters like `lv_style_set_arc_image_src` that the
# narrower regex above doesn't see.


def _load_globals_images() -> dict[str, dict]:
    """Return a mapping `name -> {src_path}` from the `<images>` section of `globals.xml`.

    Cached on first call. Keeps declaration order (Python 3.7+ dicts) for
    stable output across runs.
    """
    cache = getattr(_load_globals_images, "_cache", None)
    if cache is not None:
        return cache

    images: dict[str, dict] = {}
    if GLOBALS_XML_PATH.exists():
        tree = ET.parse(GLOBALS_XML_PATH)
        images_elem = tree.getroot().find("images")
        if images_elem is not None:
            for child in images_elem:
                if not isinstance(child.tag, str):
                    continue
                name = child.get("name")
                if not name:
                    continue
                images[name] = {
                    "src_path": child.get("src_path", ""),
                }

    _load_globals_images._cache = images  # type: ignore[attr-defined]
    return images


def _used_global_images(source: str) -> list[str]:
    """Return image names from globals.xml referenced as identifiers in `source`.

    Keeps globals.xml declaration order for stable output.
    """
    meta = _load_globals_images()
    return [n for n in meta if re.search(rf"\b{re.escape(n)}\b", source)]


def declare_global_images(source: str, path: Path) -> str:
    """Insert `LV_IMAGE_DECLARE(name)` for every globals.xml image the example references.

    Idempotent: images that already have a declare line are skipped.
    """
    meta = _load_globals_images()
    if not meta:
        return source

    used = _used_global_images(source)
    if not used:
        return source

    fn_match = FUNCTION_OPEN_BRACE_RE.search(source)
    if not fn_match:
        return source

    insert_pos = fn_match.end()
    decls: list[str] = []
    for img in used:
        if not re.search(rf"LV_IMAGE_DECLARE\s*\(\s*{re.escape(img)}\s*\)", source):
            decls.append(f"    LV_IMAGE_DECLARE({img});")

    if not decls:
        return source

    block = "\n".join(decls) + "\n\n"
    return source[:insert_pos] + block + source[insert_pos:]


# =============================================================================
# Font declarations from globals.xml
# =============================================================================
#
# The generator passes a `globals.xml`-registered font to a font setter by
# bare name: `lv_obj_set_style_text_font(label, font_example_large, 0)` or
# `lv_style_set_text_font(&style, font_example_large)`. To compile against a
# C-array font the call needs `&font_example_large` and the example must
# `LV_FONT_DECLARE(font_example_large)` so the linker resolves the extern.
#
# Scoped strictly to fonts declared in `globals.xml`'s `<fonts>` section, so
# built-in fonts (`&lv_font_montserrat_*`, `LV_FONT_DEFAULT`), theme getters
# (`lv_theme_get_font_*(...)`), and unrelated local `lv_font_t *` variables in
# C-only examples are left untouched.


def _load_globals_fonts() -> list[str]:
    """Return font names from the `<fonts>` section of `globals.xml`.

    Cached on first call. Keeps declaration order for stable output.
    """
    cache = getattr(_load_globals_fonts, "_cache", None)
    if cache is not None:
        return cache

    fonts: list[str] = []
    if GLOBALS_XML_PATH.exists():
        tree = ET.parse(GLOBALS_XML_PATH)
        fonts_elem = tree.getroot().find("fonts")
        if fonts_elem is not None:
            for child in fonts_elem:
                if not isinstance(child.tag, str):
                    continue
                name = child.get("name")
                if name and name not in fonts:
                    fonts.append(name)

    _load_globals_fonts._cache = fonts  # type: ignore[attr-defined]
    return fonts


def declare_and_ref_fonts(source: str, path: Path) -> str:
    """`&`-prefix + `LV_FONT_DECLARE` every globals.xml font the example uses.

    Idempotent: an already-`&`'d reference is left as-is, and a font that
    already has a declare line is not declared again.
    """
    names = _load_globals_fonts()
    if not names:
        return source

    used: list[str] = []  # globals order — stable output
    new_source = source
    for name in names:
        # The font name must be the *whole* argument of a text-font setter:
        # the trailing `[,)]` guard prevents matching a substring of e.g.
        # `lv_theme_get_<name>(...)`.
        ref_re = re.compile(
            r"(lv_(?:obj_set_style|style_set)_text_font\s*\(\s*[^,]+,\s*)"
            r"(&?)(" + re.escape(name) + r")(\s*[,)])"
        )

        def repl(m: re.Match) -> str:
            prefix, amp, fname, tail = m.groups()
            return m.group(0) if amp else f"{prefix}&{fname}{tail}"

        new_source, count = ref_re.subn(repl, new_source)
        if count:
            used.append(name)

    if not used:
        return new_source

    fn_match = FUNCTION_OPEN_BRACE_RE.search(new_source)
    if not fn_match:
        return new_source

    insert_pos = fn_match.end()
    decls = [
        f"    LV_FONT_DECLARE({f});"
        for f in used
        if not re.search(rf"LV_FONT_DECLARE\s*\(\s*{re.escape(f)}\s*\)", new_source)
    ]
    if not decls:
        return new_source

    block = "\n".join(decls) + "\n\n"
    return new_source[:insert_pos] + block + new_source[insert_pos:]


# =============================================================================
# Coalesce the declaration block
# =============================================================================
#
# `declare_global_images`, `declare_and_ref_images` and `declare_and_ref_fonts`
# each splice their own `\n\n`-terminated block in at the function's opening
# brace, so a file using both ends up with the font block, a blank line, then
# the image block — two stray groups in insertion order. Gather every
# `LV_*_DECLARE` line that leads the function body and re-emit them as a single
# contiguous block, images before fonts, followed by exactly one blank line —
# the hand-written convention the example tree already uses.

DECLARE_LINE_RE = re.compile(r"[ \t]*LV_(IMAGE|FONT)_DECLARE\([^)]*\);[ \t]*\Z")


def coalesce_declares(source: str, path: Path) -> str:
    fn_match = FUNCTION_OPEN_BRACE_RE.search(source)
    if not fn_match:
        return source

    start = fn_match.end()
    lines = source[start:].split("\n")

    images: list[str] = []
    fonts: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.strip() == "":
            i += 1
            continue
        m = DECLARE_LINE_RE.fullmatch(line)
        if not m:
            break
        (images if m.group(1) == "IMAGE" else fonts).append(line.strip())
        i += 1

    if not images and not fonts:
        return source

    block = "\n".join(f"    {d}" for d in images + fonts) + "\n\n"
    return source[:start] + block + "\n".join(lines[i:])


# =============================================================================
# Rename `style_inited` → `inited`
# =============================================================================
#
# After empty-block removal has done its job (which matches the literal name
# `style_inited`), rename any surviving guards. The block now also gates
# subject inits, so the more generic `inited` reads cleaner than
# `style_inited`. Whole-word match so we don't touch unrelated identifiers
# that happen to contain the substring.

STYLE_INITED_NAME_RE = re.compile(r"\bstyle_inited\b")


def rename_style_inited(source: str) -> str:
    return STYLE_INITED_NAME_RE.sub("inited", source)


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
# Drop unused pointer variables
# =============================================================================
#
# The generator assigns every created/added object to a local even when the
# example never touches it again, and not always an `lv_obj_t *` — e.g.
#
#     lv_obj_t * lv_calendar_header_arrow_0 = lv_calendar_add_header_arrow(cal);
#     lv_subject_increment_dsc_t * subject_increment_event_0 =
#         lv_obj_add_subject_increment_event(btn, &s, LV_EVENT_CLICKED, -1);
#
# The call has the side effect we want but the variable is dead. Hand-written
# examples just write the bare call. Strip the `<type> * <name> = ` prefix
# for any single-token-typed pointer local when `<name>` occurs exactly once
# in the whole file (i.e. only at its own declaration); a variable referenced
# anywhere else is kept as-is. The optional-`const` + single type token
# matches the generator's actual output without reaching for arbitrary
# multi-word C types. `static` decls can't match (the `*` must follow the
# type token directly), so subject/style scaffolding is untouched.
# Idempotent: the rewritten bare call no longer matches the pattern.

PTR_DECL_ASSIGN_RE = re.compile(
    r"^([ \t]*)(?:const\s+)?[A-Za-z_]\w*\s*\*\s*([A-Za-z_]\w*)\s*=\s*([^\n]+;)[ \t]*$",
    re.MULTILINE,
)


def drop_unused_ptr_vars(source: str) -> str:
    def repl(m: re.Match) -> str:
        indent, name, rhs = m.group(1), m.group(2), m.group(3)
        # A used variable appears at least twice (declaration + >=1 use).
        # `\b` stops `lv_calendar_0` matching inside
        # `lv_calendar_header_arrow_0`, and still counts `&name` uses.
        if len(re.findall(rf"\b{re.escape(name)}\b", source)) > 1:
            return m.group(0)
        return f"{indent}{rhs}"

    return PTR_DECL_ASSIGN_RE.sub(repl, source)


# =============================================================================
# Cast OR-ed enum flag expressions
# =============================================================================
#
# The generator emits OR-ed enum constants both in array initializers and as
# call arguments, e.g.
#
#     static const lv_buttonmatrix_ctrl_t m[] = {A, B | C, D};
#     lv_chart_add_series(c, col, LV_CHART_AXIS_PRIMARY_Y | LV_CHART_AXIS_PRIMARY_X);
#
# In C `B | C` is an `int`, which is fine; in C++ an `int` won't implicitly
# convert to the expected `enum` type, so g++ fails with "invalid conversion
# from 'int' to 'lv_..._t'". Wrap each `|`-run of constants from a *single*
# known enum family in an explicit `(lv_..._t)(...)` cast so the same source
# compiles as both C and C++.
#
# Only true `enum` types are listed. Deliberately excluded:
#   * LV_PART_* | LV_STATE_*  — combine into `lv_style_selector_t`, which is
#     `typedef uint32_t` (no enum, `int` converts cleanly). Mixed-family runs
#     are skipped anyway.
#   * LV_SCALE_LABEL_ROTATE_*  — plain `#define` int macros, no enum type.
ENUM_FLAG_TYPES = {
    "LV_BUTTONMATRIX_CTRL_": "lv_buttonmatrix_ctrl_t",
    "LV_CHART_AXIS_":        "lv_chart_axis_t",
    "LV_DIR_":               "lv_dir_t",
    "LV_FS_MODE_":           "lv_fs_mode_t",
    "LV_GRIDNAV_CTRL_":      "lv_gridnav_ctrl_t",
}

ORED_CONSTS_RE = re.compile(r"LV_[A-Z0-9_]+(?:\s*\|\s*LV_[A-Z0-9_]+)+")


def _enum_flag_type(token: str) -> str | None:
    for pfx, typ in ENUM_FLAG_TYPES.items():
        if token.startswith(pfx):
            return typ
    return None


def cast_ored_enum_flags(source: str) -> str:
    def repl(m: re.Match) -> str:
        run = m.group(0)
        types = {_enum_flag_type(t.strip()) for t in run.split("|")}
        # Cast only when every operand is from the same known enum family;
        # a mixed run (e.g. LV_PART_*|LV_STATE_*) or any unknown operand
        # leaves `types` with >1 entry or a `None`.
        if len(types) != 1 or None in types:
            return run
        typ = types.pop()
        # Idempotent: don't re-wrap a run already inside its cast.
        if source[: m.start()].endswith(f"({typ})("):
            return run
        return f"({typ})({run})"

    return ORED_CONSTS_RE.sub(repl, source)


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
#   * `replace_includes_with_lv_examples` must run before `fix_file_directive`
#     would be meaningless either way, but conceptually the include block
#     belongs to the "file identity" group.
#   * `add_top_level_doc_comment` reads from the XML and writes above the
#     function, so it must run after `rename_create_function` (so the
#     function decl is already in its final shape).
#   * `remove_blank_after_open_brace` runs last so it can clean up blanks
#     left by everything else.
TRANSFORMATIONS = [
    # Subject decls/inits — must run before the empty-block removal so the
    # block isn't yet "empty" when we want to fill it.
    init_subjects,
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
    # Image/font declarations + `&` prefix. These insert `LV_*_DECLARE`
    # lines at the function's opening brace, which `FUNCTION_OPEN_BRACE_RE`
    # only locates once the signature is the final `void <name>_create(void)`
    # — so they must run *after* `remove_return_screen` +
    # `rename_create_function`, not before.
    declare_global_images,
    declare_and_ref_images,
    declare_and_ref_fonts,
    # Merge the blocks the three transforms above each spliced in separately
    # into one contiguous, images-then-fonts declare group.
    coalesce_declares,
    lambda s, p: remove_section_banners(s),
    lambda s, p: remove_brief_line(s),
    replace_includes_with_lv_examples,
    fix_file_directive,
    # XML doc mapping.
    add_top_level_doc_comment,
    map_xml_comments,
    # Rename the guard variable now that the block has potentially gained
    # subject inits — `inited` reads better than the legacy `style_inited`.
    # Must follow `remove_empty_style_inited` which matches the literal name.
    lambda s, p: rename_style_inited(s),
    # Strip dead `<type> * x = ...;` declarations (x referenced only at
    # its own declaration). Must run after `map_xml_comments`: removing a
    # `... = lv_<type>_create(...)` LHS would otherwise desync the
    # comment-to-create pairing, which keys off `CREATE_LINE_RE`.
    lambda s, p: drop_unused_ptr_vars(s),
    # Cast OR-ed enum flag runs so examples compile under g++ too.
    lambda s, p: cast_ored_enum_flags(s),
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
