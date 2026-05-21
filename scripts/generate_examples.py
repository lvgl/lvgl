#!/usr/bin/env python3
"""Generate C files for every XML example by driving `lved-cli.js`.

Each XML example under `examples/` is run through the LVGL Pro editor's CLI
(`lved-cli.js generate <project>`), which only operates on a project tree, so
this script shuttles each example XML through `examples/xml_project/screens/`,
copies the generated `_gen.c` back beside its source XML, and finishes by
running the `cleanup_examples.py` transformations + wrapping the file in an
`#if LV_USE_<topic> && LV_BUILD_EXAMPLES` build guard.

Once every XML is processed the script rewrites each touched topic's
`lv_example_<topic>.h` (one prototype per `.c` on disk), runs
`code-format.py examples` to astyle the output, and wipes every `.c`/`.h`
file from `examples/xml_project/` so the scaffolding the CLI generates can't
collide with the host project's source globbing.

USAGE
-----
    python scripts/generate_examples.py [--cli /path/to/lved-cli.js] [paths...]

Without paths, every XML under `examples/` is processed except those inside
`examples/xml_project/` and the top-level `project.xml` / `globals.xml`.
Paths may be individual XML files or directories to recurse into.
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path

# Reuse the transformation pipeline from the sibling cleanup script instead
# of shelling out — calling `process()` directly is faster and avoids a full
# walk of `examples/` on every generated file.
sys.path.insert(0, str(Path(__file__).resolve().parent))
import cleanup_examples  # noqa: E402

REPO_ROOT = Path(__file__).resolve().parent.parent
EXAMPLES_DIR = REPO_ROOT / "examples"
PROJECT_DIR = EXAMPLES_DIR / "xml_project"
SCREENS_DIR = PROJECT_DIR / "screens"
CODE_FORMATTER = REPO_ROOT / "scripts" / "code-format.py"

# Files at the examples/ root that are project metadata, not examples.
ROOT_METADATA = {"project.xml", "globals.xml"}

# Topic folders whose name has no matching `LV_USE_<TOPIC>` macro. LVGL
# always builds scroll handling, the style system, and the base `lv_obj`
# widget in, so these examples get gated on `LV_BUILD_EXAMPLES` alone —
# emitting `LV_USE_STYLES` / `LV_USE_SCROLL` / `LV_USE_OBJ` (undefined)
# would make the `#if` false and silently drop every example in those
# folders. `get_started` is a mixed-widget intro topic with no single
# `LV_USE_GET_STARTED` macro, so it gets the same `LV_BUILD_EXAMPLES`-only
# treatment.
TOPICS_WITHOUT_LV_USE = {"scroll", "styles", "obj", "get_started"}


def find_example_xmls() -> list[Path]:
    """Return every example XML under `examples/` worth processing.

    Excludes the working project tree (`examples/xml_project/`) and the
    top-level metadata XMLs.
    """
    targets = []
    for xml in EXAMPLES_DIR.rglob("*.xml"):
        if PROJECT_DIR in xml.parents:
            continue
        if xml.parent == EXAMPLES_DIR and xml.name in ROOT_METADATA:
            continue
        targets.append(xml)
    return sorted(targets)


def resolve_cli(cli_arg: str | None) -> str:
    """Resolve the lved-cli.js path. Use --cli if given, else search PATH."""
    if cli_arg:
        p = Path(cli_arg).expanduser().resolve()
        if not p.exists():
            sys.exit(f"--cli path does not exist: {p}")
        return str(p)
    found = shutil.which("lved-cli.js")
    if found:
        return found
    sys.exit("lved-cli.js not found on PATH; pass --cli /path/to/lved-cli.js")


def _run_generator(cli_path: str) -> subprocess.CompletedProcess:
    """Run `lved-cli.js generate` against the working project tree."""
    return subprocess.run(
        ["node", cli_path, "generate", str(PROJECT_DIR.relative_to(REPO_ROOT))],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
    )


def generate_one(xml_path: Path, cli_path: str) -> bool:
    """Generate the C file for a single XML. Return True on success."""
    name = xml_path.stem  # e.g. "lv_example_flex_grow"
    staged_xml = SCREENS_DIR / xml_path.name
    generated_c = SCREENS_DIR / f"{name}_gen.c"
    generated_h = SCREENS_DIR / f"{name}_gen.h"
    target_c = xml_path.with_suffix(".c")

    SCREENS_DIR.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(xml_path, staged_xml)

    try:
        result = _run_generator(cli_path)
        if result.returncode != 0:
            sys.stderr.write(
                f"  ! lved-cli failed (exit {result.returncode}):\n{result.stderr}"
            )
            return False

        if not generated_c.exists():
            sys.stderr.write(
                f"  ! expected {generated_c.name} after generation, not found\n"
            )
            return False

        shutil.copyfile(generated_c, target_c)
        cleanup_examples.process(target_c)
        wrap_with_build_guard(xml_path, target_c)
        return True
    finally:
        # Tidy up so the screens folder doesn't accumulate per-file artifacts
        # between runs. The pre-existing test example (e.g.
        # `lv_example_dropdown_text.xml`) is untouched because its name
        # differs from whatever we staged.
        for f in (staged_xml, generated_c, generated_h):
            if f.exists():
                f.unlink()


def _topic_dir_for(xml: Path) -> Path:
    """Return the topic folder for an XML (the parent of its containing dir).

    Convention: examples live as `<topic>/<feature>/lv_example_<...>.xml`,
    so the topic folder is two levels up from the XML file itself.
    """
    return xml.parent.parent


def _guard_expr_for(xml_path: Path) -> str:
    """Return the `#if` expression for an example, derived from its topic.

    The topic folder name maps to `LV_USE_<TOPIC>` (e.g. `canvas` →
    `LV_USE_CANVAS`, `flex` → `LV_USE_FLEX`). Topics in
    `TOPICS_WITHOUT_LV_USE` have no such macro and are gated on
    `LV_BUILD_EXAMPLES` only.
    """
    topic = _topic_dir_for(xml_path).name
    if topic in TOPICS_WITHOUT_LV_USE:
        return "LV_BUILD_EXAMPLES"
    return f"LV_USE_{topic.upper()} && LV_BUILD_EXAMPLES"


# First `#include` line in the cleaned file. `cleanup_examples` collapses the
# include block to a single relative `lvgl.h`, so the first match is it; the
# `#if` is inserted right after so the guard wraps everything below the
# include (mirroring the hand-written examples).
_FIRST_INCLUDE_RE = re.compile(
    r"^#include[ \t]+[\"<][^\">\n]+[\">][ \t]*\n", re.MULTILINE
)
# Idempotency probe: an existing top-level guard already mentioning
# LV_BUILD_EXAMPLES means this file is already wrapped.
_EXISTING_GUARD_RE = re.compile(r"^#if\b.*\bLV_BUILD_EXAMPLES\b", re.MULTILINE)


def wrap_with_build_guard(xml_path: Path, c_path: Path) -> None:
    """Wrap a cleaned example C file in its `#if … #endif` build guard.

    Inserts `#if <expr>` directly after the include line and appends
    `#endif` at EOF. Idempotent: a file that already carries a
    `LV_BUILD_EXAMPLES` guard is left untouched.
    """
    text = c_path.read_text()
    if _EXISTING_GUARD_RE.search(text):
        return

    m = _FIRST_INCLUDE_RE.search(text)
    if not m:
        sys.stderr.write(
            f"  ! no #include found in {c_path.name}, skipping build guard\n"
        )
        return

    guard = f"#if {_guard_expr_for(xml_path)}\n"
    text = text[: m.end()] + guard + text[m.end():]
    c_path.write_text(text.rstrip() + "\n#endif\n")


# The per-widget header is rewritten wholesale on every run. Every prototype
# comes from a `.c` file actually present in the widget's folder — there is
# no marker / preserved-region machinery.
#
# Convention used to map `.c` file → prototype:
#   * `<topic>/lv_example_<stem>.c`            → C-only example.
#   * `<topic>/<feature>/lv_example_<stem>.c`  → XML-generated example.
# Both map to `void lv_example_<stem>(void)`.
HEADER_TEMPLATE = """\
/**
 * @file {header_name}
 *
 */

#ifndef {guard}
#define {guard}

#ifdef __cplusplus
extern "C" {{
#endif

{prototypes}

#ifdef __cplusplus
}} /*extern "C"*/
#endif

#endif /*{guard}*/
"""


def _collect_prototypes(topic_dir: Path) -> list[str]:
    """Return prototype lines for every `.c` example under `topic_dir`.

    XML-generated examples sit in feature subfolders; legacy C-only
    examples sit directly in the widget folder. Both use the bare file
    stem as the function name (`void <stem>(void)`) and appear in the same
    header so consumers don't have to know which is which.
    """
    xml_stems = {c.stem for c in topic_dir.glob("*/lv_example_*.c")}
    legacy_stems = {c.stem for c in topic_dir.glob("lv_example_*.c")}

    return [f"void {stem}(void);" for stem in sorted(xml_stems | legacy_stems)]


def write_topic_headers(touched: list[Path]) -> list[Path]:
    """Rewrite `lv_example_<topic>.h` from the `.c` files on disk.

    No markers, no preserved region. Adding or removing a `.c` file
    (XML- or hand-written) automatically reshapes the header on the next
    run.
    """
    topic_dirs = {_topic_dir_for(xml) for xml in touched}
    written: list[Path] = []
    for topic_dir in sorted(topic_dirs):
        if not topic_dir.is_dir() or EXAMPLES_DIR not in topic_dir.parents:
            continue
        topic = topic_dir.name
        prototypes = _collect_prototypes(topic_dir)
        if not prototypes:
            continue
        header_path = topic_dir / f"lv_example_{topic}.h"
        guard = f"LV_EXAMPLE_{topic.upper()}_H"
        header_path.write_text(
            HEADER_TEMPLATE.format(
                header_name=header_path.name,
                guard=guard,
                prototypes="\n".join(prototypes),
            )
        )
        written.append(header_path)
    return written


def cleanup_project_files() -> None:
    """Wipe every `.c`/`.h` file under `examples/xml_project/`.

    The CLI generates scaffolding (`xml_project.{c,h}`, `xml_project_gen.{c,h}`,
    per-screen `_gen.{c,h}`, plus font/image `_data.c` files) that would collide
    with the host project's source globbing. We don't need it between runs —
    the CLI re-creates whatever it needs on next invocation.
    """
    if not PROJECT_DIR.is_dir():
        return
    for pattern in ("*.c", "*.h"):
        for f in PROJECT_DIR.rglob(pattern):
            f.unlink()


def format_examples() -> None:
    """Run the project's astyle wrapper on the examples tree."""
    if not CODE_FORMATTER.exists():
        sys.stderr.write(
            f"  ! formatter not found at {CODE_FORMATTER}, skipping format step\n"
        )
        return
    subprocess.run(
        [sys.executable, str(CODE_FORMATTER), "examples"],
        cwd=REPO_ROOT,
    )


def regenerate_project(cli_path: str) -> bool:
    """Re-run the CLI against the now-empty `screens/` to sync project files.

    `generate_one` deletes each staged screen in its `finally` block, so once
    every example is processed `screens/` is empty again. The project-level
    generated artifacts (`xml_project_gen.{c,h}`, `file_list_gen.cmake`) still
    reference whatever screen was staged last, though. One final generation
    against the empty tree rewrites them to match real project content instead
    of a transient staged screen.

    Skipped (returning False) if `screens/` still holds staged XML, so a
    partial/interrupted run won't bake a half-state into the project files.
    """
    staged = sorted(SCREENS_DIR.glob("*.xml"))
    if staged:
        sys.stderr.write(
            "  ! screens/ still has XML, skipping final regeneration: "
            f"{', '.join(p.name for p in staged)}\n"
        )
        return False
    result = _run_generator(cli_path)
    if result.returncode != 0:
        sys.stderr.write(
            f"  ! final regeneration failed (exit {result.returncode}):\n"
            f"{result.stderr}"
        )
        return False
    return True


def collect_targets(paths: list[str]) -> list[Path]:
    """Expand user-supplied paths into a flat list of XML files."""
    out: list[Path] = []
    for raw in paths:
        p = Path(raw).resolve()
        if p.is_dir():
            out.extend(sorted(p.rglob("*.xml")))
        elif p.suffix == ".xml" and p.is_file():
            out.append(p)
        else:
            sys.stderr.write(f"skipping (not an XML or directory): {p}\n")
    return out

def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Generate C files for XML examples via lved-cli.js."
    )
    parser.add_argument(
        "--cli",
        help="path to lved-cli.js (defaults to a PATH lookup)",
    )
    parser.add_argument(
        "paths",
        nargs="*",
        help="specific XML files or directories to process; defaults to every "
             "example under examples/",
    )
    args = parser.parse_args(argv)

    cli_path = resolve_cli(args.cli)
    targets = collect_targets(args.paths) if args.paths else find_example_xmls()

    if not targets:
        print("No XML examples found.")
        return 0

    # `finally` guarantees the scaffolding cleanup runs even if generation
    # raises, so the tree never keeps globbable xml_project `.c`/`.h` behind.
    ok = 0
    try:
        for xml in targets:
            rel = xml.relative_to(REPO_ROOT) if REPO_ROOT in xml.parents else xml
            print(f"processing: {rel}")
            if generate_one(xml, cli_path):
                ok += 1

        headers = write_topic_headers(targets)
        for h in headers:
            rel = h.relative_to(REPO_ROOT) if REPO_ROOT in h.parents else h
            print(f"wrote header: {rel}")

        print("regenerating project against empty screens/ to sync _gen files")
        regenerate_project(cli_path)

        print("formatting examples/ via scripts/code-format.py")
        format_examples()
    finally:
        cleanup_project_files()

    print(f"\n{ok} of {len(targets)} examples generated.")
    return 0 if ok == len(targets) else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))