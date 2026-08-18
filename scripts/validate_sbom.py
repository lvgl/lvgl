#!/usr/bin/env python3
"""Validate LVGL's SPDX 3.0 SBOM against the official SPDX JSON Schema.

This is the validation method recommended by SPDX itself: check the document
against the published JSON Schema for its spec version. It uses check-jsonschema
(https://github.com/python-jsonschema/check-jsonschema) as the engine:

    python3 -m pip install check-jsonschema

Usage:
    python3 scripts/validate_sbom.py [SBOM ...]
        [--schema URL_OR_PATH] [--spec-version 3.0.1]

With no SBOM argument sbom/lvgl.spdx.json is validated. The schema
defaults to the official SPDX schema for --spec-version; pass a local path with
--schema to validate offline.
"""

import argparse
import glob
import importlib.util
import os
import subprocess
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SBOM_DIR = os.path.join(REPO_ROOT, "sbom")
SCHEMA_URL = "https://spdx.org/schema/%s/spdx-json-schema.json"


def main():
    parser = argparse.ArgumentParser(
        description="Validate LVGL's SPDX SBOM against the official SPDX JSON Schema.")
    parser.add_argument("sboms", nargs="*",
                        help="SBOM file(s) to validate (default: sbom/lvgl.spdx.json).")
    parser.add_argument("--spec-version", default="3.0.1",
                        help="SPDX spec version whose schema to use (default: 3.0.1).")
    parser.add_argument("--schema", default=None,
                        help="Override the schema URL or local file path.")
    args = parser.parse_args()

    sboms = args.sboms or sorted(glob.glob(os.path.join(SBOM_DIR, "lvgl.spdx.json")))
    if not sboms:
        print("No SBOM files found in %s (run scripts/generate_sbom.py first)." % SBOM_DIR)
        return 1
    missing = [p for p in sboms if not os.path.isfile(p)]
    if missing:
        print("SBOM file(s) not found: %s" % ", ".join(missing))
        return 1

    # Launching `python -m check_jsonschema` when the module is absent exits
    # with the interpreter's own error code (not FileNotFoundError), so detect
    # the module up front to surface the documented install instructions.
    if importlib.util.find_spec("check_jsonschema") is None:
        print("check-jsonschema is not installed. Install it with:\n"
              "  python3 -m pip install check-jsonschema")
        return 2

    schema = args.schema or (SCHEMA_URL % args.spec_version)

    cmd = [sys.executable, "-m", "check_jsonschema", "--schemafile", schema] + sboms
    print("Validating against SPDX %s schema:\n  %s\n" % (args.spec_version, schema))
    return subprocess.run(cmd).returncode


if __name__ == "__main__":
    sys.exit(main())
