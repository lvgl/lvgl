#!/usr/bin/env python3

import sys

try:
    import kconfiglib
except ImportError:
    print("Need kconfiglib package, do `pip3 install kconfiglib`")
    sys.exit(1)


def verify_kconfig(kconfig_file):
    kconf = kconfiglib.Kconfig(kconfig_file)

    if kconf.warnings:
        print("Warnings found:")
        for warning in kconf.warnings:
            print(warning)
        sys.exit(1)
    else:
        print("No warnings found.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python check_kconfig.py <Kconfig_file>")
        sys.exit(1)

    verify_kconfig(sys.argv[1])
