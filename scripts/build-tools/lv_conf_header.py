#!/usr/bin/env python3

# Turns the output of a preprocessor backend into the expanded configuration
# header, which generate_cmake_variables.py consumes
#
# Both backends emit the #define/#undef directives of the taken branches, so
# the same output is produced no matter which one ran

import re

# Only the configuration directives are of interest, everything else a backend
# may emit (its predefined macros, #pragma message, stray declarations left by
# the expansion of a macro) is dropped. The prefixes cover LV_*, LVGL_* and
# their CONFIG_* counterparts
DIRECTIVE_RE = re.compile(r"^#\s*(?:define|undef)\s+(?:CONFIG_)?LV")
DEFINE_NAME_RE = re.compile(r"^#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)")

# lv_conf_internal.h aligns the values of its definitions, a compiler collapses
# that padding while pcpp keeps it. Normalizing it - the padding between the
# name and the value only, never the value itself - makes both backends emit
# the very same header
PADDING_RE = re.compile(r"^(#define\s+[A-Za-z_][A-Za-z0-9_]*(?:\([^)]*\))?)\s+")


def extract_directives(lines, defs):
    """
    Keeps the configuration directives only, left aligned and with a single
    space after the #
    """
    # A backend may echo back the definitions it was given on the command line,
    # they come from the build system and are not part of the configuration
    cmdline_defs = {definition.split("=", 1)[0] for definition in defs}
    directives = []

    for line in lines:
        stripped = re.sub(r"^#\s+", "#", line.strip())

        if not DIRECTIVE_RE.match(stripped):
            continue

        name = DEFINE_NAME_RE.match(stripped)
        if name and name.group(1) in cmdline_defs:
            continue

        directives.append(PADDING_RE.sub(r"\1 ", stripped) + "\n")

    return directives


def add_include_guards(lines):
    """
    This is required - to avoid include errors when Kconfig is used and LVGL is
    installed on the system - i.e when lvgl.h is used as a system include
    """
    lines.insert(0, "#define LV_CONF_H\n\n")
    lines.insert(0, "#ifndef LV_CONF_H\n")
    lines.append("#endif /* END LV_CONF_H */\n")

    return lines


def generate(preprocessed_file, output_file, defs):
    with open(preprocessed_file, "r") as f:
        lines = f.readlines()

    lines = add_include_guards(extract_directives(lines, defs))

    with open(output_file, "w") as f:
        f.writelines(lines)
