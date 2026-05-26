#!/usr/bin/env python3
"""Wrap XML files as Markdown so they can be consumed by systems that accept
.md but not .xml. Each `foo.xml` becomes `foo.md` with its contents wrapped in
a fenced code block titled with the original filename, optionally annotated with
a `source` URL pointing back at the original .xml:

    ```xml title="foo.xml" source="https://.../foo.xml"
    <the original xml>
    ```

The `source` URL is built from a base you pass on the CLI plus the file's path
within the source tree. For example, with

    --source-base https://github.com/lvgl/lvgl/blob/master/examples

a file at SRC_DIR/scroll/scroll_scrollable/lv_example_scroll_scrollable.xml gets

    source="https://github.com/lvgl/lvgl/blob/master/examples/scroll/scroll_scrollable/lv_example_scroll_scrollable.xml"

Two ways to call it:

  Single file (in-place rename, original .xml is removed):
    python3 xml_to_md.py path/to/file.xml [--source-base URL]

  Folder (recursive, mirrored to an output dir, source left untouched):
    python3 xml_to_md.py SRC_DIR OUT_DIR [--source-base URL]

The folder mode walks SRC_DIR recursively. For every .xml it finds it recreates
the matching subfolder under OUT_DIR and writes the .md there. Subfolders that
contain no XML are never created. Designed to run unattended in CI.

Standalone: depends only on the Python standard library (Python 3.8+).
"""

from __future__ import annotations

import os
import re
import sys


def to_markdown(xml: str, title: str, source: str | None) -> str:
    # Ensure exactly one trailing newline before the closing fence.
    body = re.sub(r"\n+$", "", xml)
    attrs = f'title="{title}"'
    if source is not None:
        attrs += f' source="{source}"'
    return f"```xml {attrs}\n{body}\n```\n"


def build_source(source_base: str | None, rel: str) -> str | None:
    """Join the CLI-provided base URL with the file's relative path (always
    using forward slashes, since this is a URL). Returns None if no base set."""
    if source_base is None:
        return None
    return f"{source_base.rstrip('/')}/{rel.replace(os.sep, '/')}"


def convert_file(src: str, out_path: str, source: str | None) -> None:
    """Read XML at `src`, write the wrapped Markdown to `out_path`.

    The fence title is always the original .xml filename and is never
    overridden. Parent folders of `out_path` are created as needed.
    """
    with open(src, "r", encoding="utf-8") as f:
        xml = f.read()

    parent = os.path.dirname(out_path)
    if parent:
        os.makedirs(parent, exist_ok=True)

    with open(out_path, "w", encoding="utf-8") as f:
        f.write(to_markdown(xml, src, source))


def convert_tree(src_dir: str, out_dir: str, source_base: str | None) -> int:
    """Recursively convert every .xml under `src_dir` into `out_dir`, mirroring
    the folder structure. Only folders that contain XML are created. Returns the
    number of files converted. The source tree is left untouched."""
    converted = 0
    for root, _, files in os.walk(src_dir):
        for name in files:
            if not name.lower().endswith(".xml"):
                continue
            src = os.path.join(root, name)
            rel = os.path.relpath(src, src_dir)
            out_path = os.path.join(
                out_dir, re.sub(r"\.xml$", ".md", rel, flags=re.IGNORECASE)
            )
            convert_file(src, out_path, build_source(source_base, rel))
            print(f"{src} -> {out_path}")
            converted += 1
    return converted


def parse_args(argv: list[str]):
    """Pull out --source-base / -s and return (positionals, source_base)."""
    positionals: list[str] = []
    source_base: str | None = None
    i = 0
    while i < len(argv):
        arg = argv[i]
        if arg in ("--source-base", "-s"):
            i += 1
            if i >= len(argv):
                print(f"{arg} needs a value", file=sys.stderr)
                sys.exit(1)
            source_base = argv[i]
        elif arg.startswith("--source-base="):
            source_base = arg.split("=", 1)[1]
        elif arg.startswith("-"):
            print(f"Unknown flag: {arg}", file=sys.stderr)
            sys.exit(1)
        else:
            positionals.append(arg)
        i += 1
    return positionals, source_base


def usage() -> None:
    print(
        "Usage:\n"
        "  python3 xml_to_md.py <file.xml> [--source-base URL]            (in-place rename)\n"
        "  python3 xml_to_md.py <src_dir> <out_dir> [--source-base URL]   (recursive, mirrored)",
        file=sys.stderr,
    )
    sys.exit(1)


def main() -> None:
    args, source_base = parse_args(sys.argv[1:])

    if not args or len(args) > 2:
        usage()

    src = args[0]

    if os.path.isdir(src):
        if len(args) != 2:
            usage()
        converted = convert_tree(src, args[1], source_base)
    elif os.path.isfile(src):
        if not src.lower().endswith(".xml"):
            print(f"Not an .xml file: {src}", file=sys.stderr)
            sys.exit(1)
        source = build_source(source_base, src)
        if len(args) == 2:
            # Explicit destination given: keep the source, write .md there.
            out_path = args[1]
            if os.path.isdir(out_path) or out_path.endswith(os.sep):
                out_path = os.path.join(
                    out_path,
                    re.sub(
                        r"\.xml$", ".md", os.path.basename(src), flags=re.IGNORECASE
                    ),
                )
            convert_file(src, out_path, source)
        else:
            # No destination: in-place rename, removing the original .xml.
            out_path = re.sub(r"\.xml$", ".md", src, flags=re.IGNORECASE)
            convert_file(src, out_path, source)
            if out_path != src:
                os.remove(src)
        print(f"{src} -> {out_path}")
        converted = 1
    else:
        print(f"Not a file or directory: {src}", file=sys.stderr)
        sys.exit(1)

    print(f"\nDone. Converted {converted} file{'' if converted == 1 else 's'}.")


if __name__ == "__main__":
    main()
