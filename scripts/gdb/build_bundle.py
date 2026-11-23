#!/usr/bin/env python3
"""
Pack the whole GDB plugin into one file, bundle/lvglgdb.py.

Users download that single file and load it with `source lvglgdb.py` in GDB,
without cloning LVGL or installing anything. The bundle holds every file of
the plugin as a string literal. When sourced, it writes them to a per-user
cache directory and imports the package from there, so the plugin runs exactly
as it does from a checkout, data files included.

The bundle is not committed. The gdb_bundle_release workflow builds it and
attaches it to every GitHub release. To build it locally:

    python3 build_bundle.py [output.py]

The output defaults to bundle/lvglgdb.py (git-ignored).
"""

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUTPUT = ROOT / "bundle" / "lvglgdb.py"

# Paths relative to ROOT. The plugin reads widget_specs.json from
# scripts/generators/ at runtime, so it travels with the package at the same
# relative location.
SOURCES = ["lvglgdb", "scripts/generators/widget_specs.json"]


def collect() -> dict:
    files = {}
    for entry in SOURCES:
        path = ROOT / entry
        paths = [path] if path.is_file() else sorted(path.rglob("*"))
        for p in paths:
            if not p.is_file() or "__pycache__" in p.parts or p.suffix == ".pyc":
                continue
            rel = p.relative_to(ROOT).as_posix()
            text = p.read_bytes().decode("utf-8")
            if "\r" in text:
                sys.exit(f"error: {rel} has CR line endings; convert it to LF")
            files[rel] = text
    return dict(sorted(files.items()))


def literal(text: str) -> str:
    """A triple-quoted string literal that evaluates back to `text`.

    Kept multi-line on purpose, so that diffs of the bundle stay readable.
    """
    body = text.replace("\\", "\\\\").replace('"""', '\\"\\"\\"')
    if body.endswith('"'):
        body = body[:-1] + '\\"'
    return '"""' + body + '"""'


def version() -> str:
    text = (ROOT / "lvglgdb" / "_version.py").read_text(encoding="utf-8")
    return re.search(r'__version__\s*=\s*"([^"]+)"', text).group(1)


LOADER = '''

def _load():
    import os
    import sys

    if sys.platform == "win32":
        base = os.environ.get("LOCALAPPDATA") or os.path.expanduser("~")
    else:
        base = os.environ.get("XDG_CACHE_HOME") or os.path.expanduser("~/.cache")
    # Per user, not the shared temp directory, so nobody else can plant code
    # there; and per content hash, so different versions never mix.
    root = os.path.join(base, "lvglgdb", f"{VERSION}-{DIGEST}")
    done = os.path.join(root, ".complete")

    if not os.path.exists(done):
        for rel, text in FILES.items():
            path = os.path.join(root, *rel.split("/"))
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8", newline="\\n") as f:
                f.write(text)
        open(done, "w").close()

    # Sourcing the bundle again reloads the plugin, like gdbinit.py does.
    for name in [m for m in sys.modules if m == "lvglgdb" or m.startswith("lvglgdb.")]:
        del sys.modules[name]
    if root in sys.path:
        sys.path.remove(root)
    sys.path.insert(0, root)

    import lvglgdb  # noqa: F401


_load()
'''


def build() -> str:
    files = collect()
    digest = hashlib.sha256(
        "".join(f"{k}\0{v}\0" for k, v in files.items()).encode("utf-8")
    ).hexdigest()[:16]

    out = [
        "# LVGL GDB plugin, packed into a single file.",
        "#",
        "# Load it in GDB with:",
        "#     source lvglgdb.py",
        "#",
        "# GENERATED FILE - do not edit. The source is lvgl/scripts/gdb/lvglgdb/;",
        "# build it with: python3 scripts/gdb/build_bundle.py",
        "# fmt: off",
        "",
        f'VERSION = "{version()}"',
        f'DIGEST = "{digest}"',
        "",
        "FILES = {}",
    ]
    for rel, text in files.items():
        out.append("")
        out.append(f"FILES[{rel!r}] = {literal(text)}")
    return "\n".join(out) + "\n" + LOADER


def main():
    output = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else OUTPUT
    output.parent.mkdir(parents=True, exist_ok=True)
    content = build()
    output.write_bytes(content.encode("utf-8"))
    print(f"Wrote {output} ({len(content) // 1024} KiB)")


if __name__ == "__main__":
    main()
