#!/usr/bin/env python3
"""Generate C files for every XML example by driving `lved-cli.js`.

PURPOSE
-------
Each XML example under `examples/` is turned into a C file by the LVGL Pro
editor's CLI (`lved-cli.js generate <project>`). The CLI only operates on a
project tree, so this script shuttles each example XML through
`examples/xml_project/screens/`:

  1. Copy `<...>/lv_example_foo.xml` into `examples/xml_project/screens/`.
  2. Run `lved-cli.js generate examples/xml_project` from the repo root.
  3. Copy the generated `lv_example_foo_gen.c` back next to the source XML,
     renamed to `lv_example_foo.c` so `cleanup_examples.py` will pick it up
     (it requires `.c` and `.xml` siblings with matching basenames).
  4. Run the cleanup transformations on that `.c` in-process.
  5. Remove the staged XML and its generated outputs from `screens/` so
     re-runs start clean.

After all XMLs in a run are processed, each touched "topic" folder (the
parent of the XML's containing dir — e.g. `examples/layouts/flex/` for
`flex/flex_grow/lv_example_flex_grow.xml`) gets a `lv_example_<topic>.h`
header rewritten with `void <stem>_create(void);` prototypes for every
example XML in that subtree. Headers are always overwritten.

Existing sibling `.c` files are always overwritten.

USAGE
-----
    python scripts/generate_examples.py [--cli /path/to/lved-cli.js] [paths...]

Without paths, every XML under `examples/` is processed except those inside
`examples/xml_project/` and the top-level `project.xml` / `globals.xml`.
Paths may be individual XML files or directories to recurse into.
"""

from __future__ import annotations

import argparse
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

# Files at the examples/ root that are project metadata, not examples.
ROOT_METADATA = {"project.xml", "globals.xml"}

# Project scaffolding C/H files. They define globals (subjects, images,
# fonts) that collide with the host example project when it builds with
# source globbing — and `xml_project/` is globbed too. They are kept on
# disk with a `.txt` suffix so the glob skips them, and unmasked to real
# `.c`/`.h` only while lved-cli runs (it needs them), then re-masked.
MASKED_PROJECT_FILES = [
    PROJECT_DIR / "xml_project.c",
    PROJECT_DIR / "xml_project.h",
    PROJECT_DIR / "xml_project_gen.c",
    PROJECT_DIR / "xml_project_gen.h",
]
MASK_SUFFIX = ".txt"


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


# The per-widget header is rewritten wholesale on every run. Every prototype
# comes from a `.c` file actually present in the widget's folder — there is
# no marker / preserved-region machinery.
#
# Convention used to map `.c` file → prototype:
#   * `<topic>/lv_example_<stem>.c`            → C-only example,
#                                                `void lv_example_<stem>(void)`.
#   * `<topic>/<feature>/lv_example_<stem>.c`  → XML-generated example,
#                                                `void lv_example_<stem>_create(void)`.
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

    XML-generated examples sit in feature subfolders and carry the `_create`
    suffix; legacy C-only examples sit directly in the widget folder and
    use the file stem as the function name. Both kinds appear in the same
    header so consumers don't have to know which is which.
    """
    xml_stems = sorted({c.stem for c in topic_dir.glob("*/lv_example_*.c")})
    legacy_stems = sorted({c.stem for c in topic_dir.glob("lv_example_*.c")})

    out = [f"void {stem}_create(void);" for stem in xml_stems]
    out += [f"void {stem}(void);" for stem in legacy_stems]
    return out


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


def unmask_project_files() -> None:
    """Strip the `.txt` guard so lved-cli sees real `.c`/`.h` files.

    Tolerant of a partial state: if the masked file is missing but a real
    one already exists (e.g. the very first run, before masking was ever
    applied), it is left as-is and `mask_project_files()` will guard it
    afterwards.
    """
    for real in MASKED_PROJECT_FILES:
        masked = real.with_name(real.name + MASK_SUFFIX)
        if masked.exists():
            masked.replace(real)


def mask_project_files() -> None:
    """Re-add the `.txt` guard so host-project C globbing skips these files.

    Run from a `finally` so an interrupted generation can never leave
    globbable scaffolding behind.
    """
    for real in MASKED_PROJECT_FILES:
        if real.exists():
            real.replace(real.with_name(real.name + MASK_SUFFIX))


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

    # Unmask the scaffolding for the duration of generation; the `finally`
    # re-masks it even if a generation step raises, so the tree never keeps
    # globbable project files.
    ok = 0
    unmask_project_files()
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
    finally:
        mask_project_files()

    print(f"\n{ok} of {len(targets)} examples generated.")
    return 0 if ok == len(targets) else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))