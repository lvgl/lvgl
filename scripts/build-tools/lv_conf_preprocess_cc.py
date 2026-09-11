#!/usr/bin/env python3

# Expands the configuration with the C compiler used for the build
#
# The -dD option emits the #define/#undef directives of the taken branches, in
# source order, which is what the expanded configuration header is made of

import os
import re
import subprocess
import tempfile

NAME = "the C compiler"


def build_command(cc, input_file, output_file, include_dirs, defs):
    """
    -dD    emit the #define/#undef directives along with the preprocessed output
    -P     omit the linemarkers
    -nostdinc
           the system include directories must not be searched, otherwise an
           lv_conf.h of an LVGL installed on the system is picked up by the
           __has_include() check of lv_conf_internal.h
    """
    command = [cc, "-E", "-dD", "-P", "-nostdinc", "-x", "c", "-o", output_file]

    for include_path in include_dirs:
        command.append(f"-I{include_path}")

    for definition in defs:
        command.append(f"-D{definition}")

    command.append(input_file)

    return command


def is_available(cc):
    """
    Checks that the compiler keeps the #define directives in its output. MSVC
    has no equivalent of -dD, it can't be used to expand the configuration
    """
    if not cc:
        return False

    with tempfile.TemporaryDirectory() as tmp_dir:
        probe_input = os.path.join(tmp_dir, "probe.h")
        probe_output = os.path.join(tmp_dir, "probe.i")

        with open(probe_input, "w") as f:
            f.write("#define LV_PROBE 1\n")

        try:
            subprocess.run(
                build_command(cc, probe_input, probe_output, [], []),
                check=True,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )

            with open(probe_output, "r") as f:
                return any(
                    re.match(r"^\s*#\s*define\s+LV_PROBE\s+1", line) for line in f
                )

        except (subprocess.CalledProcessError, OSError):
            return False


def preprocess(cc, input_file, output_file, include_dirs, defs):
    subprocess.run(
        build_command(cc, input_file, output_file, include_dirs, defs), check=True
    )
