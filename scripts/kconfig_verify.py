#!/usr/bin/env python3

import sys
import os
from pathlib import Path

try:
    import kconfiglib
except ImportError:
    print("Need kconfiglib package, do `pip3 install kconfiglib`")
    sys.exit(1)

SCRIPT_PATH = Path(__file__).resolve()
LVGL_DIR = SCRIPT_PATH.parent.parent
KCONFIG_PATH = os.path.join(LVGL_DIR, "Kconfig")


def verify_kconfig(kconfig_file):
    if not "LVGL_DIR" in os.environ:
        os.environ["LVGL_DIR"] = str(LVGL_DIR)

    kconf = kconfiglib.Kconfig(kconfig_file)

    if kconf.warnings:
        print("Warnings found:")
        for warning in kconf.warnings:
            print(warning)
        sys.exit(1)
    else:
        print("No warnings found.")


def check_kconfig_spaces(file_path):
    """Check for space-only indentation in a Kconfig file.

    Tab+spaces is allowed (conventional for help text).
    Pure-space indentation will break kernel/Buildroot Kconfig parsers.
    """
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            lines = f.readlines()
        violations = []
        for line_num, line in enumerate(lines, 1):
            stripped = line.lstrip()
            if not stripped:
                continue
            indent = line[: len(line) - len(stripped)]
            if indent and not indent.startswith("\t") and " " in indent:
                violations.append((line_num, indent))
        if violations:
            print(f"Space-only indentation found in {file_path}:")
            for line_num, indent in violations:
                print(f"  Line {line_num}: {indent.count(' ')} leading spaces")
    except Exception as e:
        print(f"Error processing {file_path}: {e}")


def recurse(path):
    entries = os.listdir(path)
    for entry in entries:
        abs = os.path.join(path, entry)
        if os.path.isdir(abs):
            recurse(abs)
        if entry.startswith("Kconfig"):
            check_kconfig_spaces(abs)


if __name__ == "__main__":
    verify_kconfig(KCONFIG_PATH)
    recurse(LVGL_DIR)
