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


def check_kconfig_spaces(file_path):
    """Check for space-based indentation in a Kconfig file"""
    try:
        # Read file content to check for spaces
        with open(file_path, "r", encoding="utf-8") as f:
            lines = f.readlines()

        space_indent_lines = []
        for line_num, line in enumerate(lines, 1):
            # Check for leading spaces (skip empty lines)
            stripped_line = line.lstrip()
            if stripped_line and len(line) > len(stripped_line):
                # Extract the indentation part
                indent = line[: -len(stripped_line)] if stripped_line else line
                if " " in indent:
                    space_indent_lines.append((line_num, indent))

        if space_indent_lines:
            print(f"Space-based indentation found in file {file_path}:")
            for line_num, indent in space_indent_lines:
                print(f"Line {line_num}: Indent contains {indent.count(' ')} spaces")

            sys.exit(1)
        else:
            print(f"No space-based indentation found in file {file_path}")

    except Exception as e:
        print(f"Error processing file {file_path}: {e}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <Kconfig_file>")
        sys.exit(1)

    verify_kconfig(sys.argv[1])
    check_kconfig_spaces(sys.argv[1])
