#!/usr/bin/env python3

#
# Generate the cmake variables CONFIG_LV_USE_* or CONFIG_LV_BUILD_* from the
# preprocessed lv_conf_internal.h
#
# Author: David TRUAN (david.truan@edgemtech.ch)
# Author: Erik Tagirov (erik.tagirov@edgemtech.ch)
#

import os
import argparse
import re

def fatal(msg):
    print()
    print("ERROR! " + msg)
    exit(1)

def get_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description=""
                                     "Convert the expanded lv_conf_internal.h to cmake variables."
                                     "It converts all LV_USE_*, LV_BUILD_* configurations."
                                    )

    parser.add_argument('--input', type=str, required=True, nargs='?',
                        help='Path of the macro expanded lv_conf_internal.h, which should be generated during a cmake build')

    parser.add_argument('--output', type=str, required=True, nargs='?',
                        help='Path of the output file, where the cmake variables declaration will be written (ex: build/lv_conf.cmake)')

    parser.add_argument("--kconfig", action="store_true", help="Enable kconfig flag")

    parser.add_argument("--debug", action="store_true", required=False, help="Show unhandled expressions")

    parser.add_argument("--parentscope", action="store_true", required=False, help="Additionally set the variables in the parent scope")

    args = parser.parse_args()

    # The input must exist
    if not os.path.exists(args.input):
        fatal(f"Input {args.input} not found")

    return args

def write_set_cmd(fout, expr, is_parent_scope):

    fout.write(f'set({expr})\n')

    # This makes the variable usable from the top level directory
    if is_parent_scope == True:
        fout.write(f'set({expr} PARENT_SCOPE)\n')

def generate_cmake_variables(path_input: str, path_output: str, kconfig: bool, debug: bool, is_parent_scope: bool):
    fin = open(path_input)
    fout = open(path_output, "w", newline='')

    # If we use Kconfig, we must check the CONFIG_LV_USE_* and 
    # CONFIG_LV_BUILD_* defines
    if kconfig:
        CONFIG_PATTERN="^#define +(CONFIG_LV_USE|CONFIG_LV_BUILD|CONFIG_LV_[0-9A-Z_]+_USE)"
        CONFIG_PREFIX=""
    # Otherwise check the LV_USE_* and LV_BUILD_* defines
    else:
        CONFIG_PATTERN="^#define +(LV_USE|LV_BUILD|LV_[0-9A-Z_]+_USE)"
        CONFIG_PREFIX="CONFIG_"


    # Using the expanded lv_conf_internal, we don't have to deal with regexp,
    # as all the #define will be aligned on the left with a single space before the value
    for line in fin.read().splitlines():

        # Treat the LV_USE_STDLIB_* configs in a special way, as we need
        # to convert the define to full config with 1 value when enabled
        if re.search(f'{CONFIG_PATTERN}_STDLIB', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_STDLIB_")[1]

            name = name.replace("STDLIB", type)

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} 1', is_parent_scope)

        # Treat the LV_USE_OS config in a special way, as we need
        # to convert the define to full config with 1 value when enabled
        if re.search(f'{CONFIG_PATTERN}_OS', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_OS")[1]

            name += type

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} 1', is_parent_scope)

        # For the rest of the configs, simply add CONFIG_ and write the name of the define
        # all LV_USE_* or LV_BUILD_* configs where the value is 0 or 1,
        # as these are the ones that are needed in cmake
        # To detect the configuration of LVGL to perform conditional compilation/linking
        elif re.search(f'{CONFIG_PATTERN}.* +[01] *$', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} {value}', is_parent_scope)

        else:
            # Useful for debugging expressions that are unhandled,
            # if the script fails in 'unexpected ways'
            if debug == True:
                print(f"DBG: Skipping expression: '{line} - not handled'")


if __name__ == '__main__':
    args = get_args()

    generate_cmake_variables(args.input, args.output, args.kconfig, args.debug, args.parentscope)
