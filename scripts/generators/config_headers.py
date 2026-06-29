"""CLI: generate the configuration headers from a Kconfig tree.

    python3 -m config_headers [Kconfig] --template PATH --internal PATH

Run from the ``scripts/generators`` directory (so ``config_headers`` is
importable), or with that directory on ``PYTHONPATH``.
"""

from __future__ import annotations

import argparse
import os
import sys

from config_headers.emit import generate_bridge, generate_internal, generate_template
from config_headers.parse import load, parse_entries


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(prog="config_headers", allow_abbrev=False)
    ap.add_argument(
        "kconfig", nargs="?", default="Kconfig", help="Top-level Kconfig file"
    )
    ap.add_argument("--template", help="Write lv_conf_template.h here")
    ap.add_argument("--internal", help="Write lv_conf_internal.h here")
    ap.add_argument("--bridge", help="Write lv_conf_kconfig.h here")
    ap.add_argument("--srctree", help="Source root for resolving `source` directives")
    args = ap.parse_args(argv)

    # abspath so kconfiglib doesn't join a relative path onto $srctree twice.
    kconfig_path = os.path.abspath(args.kconfig)
    lvgl_dir = os.path.dirname(kconfig_path)
    if not "LVGL_DIR" in os.environ:
        os.environ["LVGL_DIR"] = lvgl_dir

    kconf = load(kconfig_path, args.srctree)
    entries = parse_entries(kconf)

    wrote = False
    if args.template:
        with open(args.template, "w") as f:
            f.write(generate_template(kconf, entries))
        print(f"wrote {args.template}", file=sys.stderr)
        wrote = True
    if args.internal:
        with open(args.internal, "w") as f:
            f.write(generate_internal(kconf, entries))
        print(f"wrote {args.internal}", file=sys.stderr)
        wrote = True
    if args.bridge:
        with open(args.bridge, "w") as f:
            f.write(generate_bridge(kconf, entries))
        print(f"wrote {args.bridge}", file=sys.stderr)
        wrote = True

    if not wrote:
        sys.stdout.write(generate_template(kconf, entries))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
