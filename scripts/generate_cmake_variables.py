#!/usr/bin/env python3

#
# Generate the cmake variables CONFIG_LV_USE_* from the
# preprocessed lv_conf_internal.h
#
# Author: David TRUAN (david.truan@edgemtech.ch)
#

import os
import argparse

def fatal(msg):
    print()
    print("ERROR! " + msg)
    exit(1)

def get_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description=""
                                     "Convert the expanded lv_conf_internal.h to cmake variables."
                                     "It converts all LV_USE_* configurations."
                                    )

    parser.add_argument('--input', type=str, required=True, nargs='?',
                        help='Path of the macro expanded lv_conf_internal.h, which should be generated during a cmake build')

    parser.add_argument('--output', type=str, required=True, nargs='?',
                        help='Path of the output file, where the cmake variables declaration will be written (ex: build/lv_conf.cmake)')

    parser.add_argument("--kconfig", action="store_true", help="Enable kconfig flag")

    args = parser.parse_args()

    # The input must exist
    if not os.path.exists(args.input):
        fatal(f"Input {args.input} not found")

    return args

def generate_cmake_variables(path_input: str, path_output: str, kconfig: bool):
    fin = open(path_input)
    fout = open(path_output, "w", newline='')

    # If we use Kconfig, we must check the CONFIG_LV_USE_* defines
    if kconfig:
        CONFIG_PATTERN="#define CONFIG_LV_USE"
        CONFIG_PREFIX=""
    # Otherwise check the LV_USE_* defines
    else:
        CONFIG_PATTERN="#define LV_USE"
        CONFIG_PREFIX="CONFIG_"


    # Using the expanded lv_conf_internal, we don't have to deal with regexp,
    # as all the #define will be aligned on the left with a single space before the value
    for line in fin.read().splitlines():

        # Treat the LV_USE_STDLIB_* configs in a special way, as we need
        # to convert the define to full config with 1 value when enabled
        if line.startswith(f'{CONFIG_PATTERN}_STDLIB'):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_STDLIB_")[1]

            name = name.replace("STDLIB", type)

            fout.write(f'set({CONFIG_PREFIX}{name} 1)\n')

        # Treat the LV_USE_OS config in a special way, as we need
        # to convert the define to full config with 1 value when enabled
        if line.startswith(f'{CONFIG_PATTERN}_OS'):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_OS")[1]

            name += type

            fout.write(f'set({CONFIG_PREFIX}{name} 1)\n')

        # For the rest of config, simply add CONFIG_ and write
        # all LV_USE_* configs, as these are the one needed in cmake
        elif line.startswith(f'{CONFIG_PATTERN}'):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            fout.write(f'set({CONFIG_PREFIX}{name} {value})\n')


if __name__ == '__main__':
    args = get_args()

    generate_cmake_variables(args.input, args.output, args.kconfig)