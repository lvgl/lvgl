"""CLI: generate the LVGL configuration files.

    python3 config_files.py
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import sys
import re

from config_headers.emit import generate_bridge, generate_internal, generate_template
from config_headers.parse import load, parse_entries

SCRIPT_PATH = Path(__file__).resolve()

ROOT_FILES = [
    "src/stdlib/Kconfig",
    "src/osal/Kconfig",
    "src/draw/Kconfig",
    "src/indev/Kconfig",
    "src/core/Kconfig",
    "src/logging/Kconfig",
    "src/themes/Kconfig",
    "src/layouts/Kconfig",
    "src/image/Kconfig",
    "src/font/Kconfig",
    "src/widgets/Kconfig",
    "src/drivers/Kconfig",
    "src/fs/Kconfig",
    "src/debugging/Kconfig",
    "src/others/Kconfig",
    "env_support/kconfig/Kconfig.build",
    "env_support/kconfig/Kconfig.compiler",
    "examples/Kconfig",
    "demos/Kconfig",
]


def parse_kconfig_content(path):
    with open(path, "r") as f:
        lines = f.readlines()

    SOURCE_RE = re.compile(r'^(?!.*#.*\brsource\b)\s*rsource\s+"([^"]+)"')

    content = []
    for line in lines:
        m = re.match(SOURCE_RE, line)
        if not m:
            content.append(line)
            continue
        relpath = m.group(1)
        dir_name = os.path.dirname(path)
        sourced_path = os.path.join(dir_name, relpath)
        content.extend(parse_kconfig_content(sourced_path))

    return content


def generate_kconfig_content(lvgl_root_dir):
    content = ["# Generated file. DO NOT EDIT!\n\n"]
    content += ['menu "LVGL"\n']

    for root_file in ROOT_FILES:
        abs_path = os.path.join(lvgl_root_dir, root_file)
        content.extend(parse_kconfig_content(abs_path))

    content += ['\nendmenu #"LVGL"\n']
    return content


def main() -> int:

    lvgl_root_dir = SCRIPT_PATH.parents[2]
    lv_conf_template = os.path.join(lvgl_root_dir, "lv_conf_template.h")
    lv_conf_internal = os.path.join(
        lvgl_root_dir, "include", "lvgl", "config", "lv_conf_internal.h"
    )
    lv_conf_kconfig = os.path.join(
        lvgl_root_dir, "include", "lvgl", "config", "lv_conf_kconfig.h"
    )

    if not "LVGL_DIR" in os.environ:
        os.environ["LVGL_DIR"] = str(lvgl_root_dir)

    kconfig_content = generate_kconfig_content(lvgl_root_dir)
    kconfig_file_path = os.path.join(lvgl_root_dir, "Kconfig")
    with open(kconfig_file_path, "w") as f:
        f.writelines(kconfig_content)

    kconf = load(kconfig_file_path)
    entries = parse_entries(kconf)

    with open(lv_conf_template, "w") as f:
        f.write(generate_template(kconf, entries))
    with open(lv_conf_internal, "w") as f:
        f.write(generate_internal(kconf, entries))
    with open(lv_conf_kconfig, "w") as f:
        f.write(generate_bridge(kconf, entries))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
