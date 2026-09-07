from kconfiglib import Kconfig, KconfigError, BOOL, TRISTATE, TRI_TO_STR

from pathlib import Path
import os
import sys

SCRIPT_PATH = Path(__file__).resolve()
LVGL_ROOT_DIR = SCRIPT_PATH.parents[2]
KCONFIG = os.path.join(LVGL_ROOT_DIR, "Kconfig")
DEFCONFIG_DIR = os.path.join(LVGL_ROOT_DIR, "configs", "defconfigs")


def check_defconfig(kconfig: Kconfig, defconfig):
    failed = False
    n = len(kconfig.warnings)
    kconfig.load_config(defconfig)
    load_warnings = kconfig.warnings[n:]

    for warning in load_warnings:
        print(f"\t{warning}")
        failed = True

    # 1. Assignments to symbols that don't exist
    for name, val in kconfig.missing_syms:
        print(f"\tundefined symbol: CONFIG_{name}={val}")
        failed = True

    # 2. Assignments that had no effect (deps unmet, or clamped by a range)
    for sym in kconfig.unique_defined_syms:
        if sym.user_value is None:
            continue
        want = TRI_TO_STR[sym.user_value] if sym.orig_type is BOOL else sym.user_value
        got = sym.str_value
        if want != got:
            print(f"\tCONFIG_{sym.name}: wanted {want}, got {got}")
            print(f"\t{sym}")
            failed = True

    return failed


def main() -> int:
    try:
        kconf = Kconfig(KCONFIG, warn=True, warn_to_stderr=False)
    except KconfigError as e:
        print(e)
        raise SystemExit(1)

    kconf.warn_assign_undef = True
    kconf.warn_assign_override = True
    kconf.warn_assign_redun = True

    defconfigs = os.listdir(DEFCONFIG_DIR)
    ret = 0

    for defconf in defconfigs:
        defconfig = os.path.join(DEFCONFIG_DIR, defconf)
        print(f"=" * 80)
        print(f"Parsing {defconfig}")
        print(f"=" * 80)
        if check_defconfig(kconf, defconfig):
            ret = 1
        else:
            print("\tOK")
    return ret


if __name__ == "__main__":
    sys.exit(main())
