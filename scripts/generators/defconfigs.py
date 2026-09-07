#!/usr/bin/env python3
"""
Generate an empty defconfigs in configs/defconfig/empty.defconfig in which almost everything is disabled.
It takes the default LVGL configuration and disables everything except a few critical configs
"""

import os
import re
from pathlib import Path

from kconfiglib import BOOL, Kconfig

SCRIPT_PATH = Path(__file__).resolve()
LVGL_ROOT_DIR = SCRIPT_PATH.parents[2]
KCONFIG = os.path.join(LVGL_ROOT_DIR, "Kconfig")
EMPTY_DEFCONFIG = os.path.join(
    LVGL_ROOT_DIR, "configs", "defconfigs", "empty.defconfig"
)


def generate_empty_defconfig(kconfig, defconfig):
    CONFIGS_TO_KEEP = {
        "LV_CONF_SKIP",
        "LV_USE_CHECK_ARG",
        "LV_USE_DRAW_SW",
        "LV_DRAW_SW*",
    }
    kconfig = Kconfig(kconfig)

    for symbol in kconfig.defined_syms:
        if (
            not any(re.match(x, symbol.name) for x in CONFIGS_TO_KEEP)
            and symbol.assignable
            and symbol.type == BOOL
            and symbol.str_value == "y"
        ):
            symbol.set_value("n")

    kconfig.write_min_config(defconfig)


def main():
    generate_empty_defconfig(KCONFIG, EMPTY_DEFCONFIG)


if __name__ == "__main__":
    main()
