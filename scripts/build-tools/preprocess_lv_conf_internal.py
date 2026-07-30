#!/usr/bin/env python3

# Preprocess the lv_conf_internal.h to generate a header file containing the
# evaluated definitions. This output will be used to generate the cmake
# variables
#
# The C compiler of the build does the preprocessing. When it can't emit its
# #define directives - which is the case of MSVC - pcpp is used instead, if it
# is available

import argparse
import os
import subprocess

import lv_conf_header
import lv_conf_preprocess_cc as cc_backend
import lv_conf_preprocess_pcpp as pcpp_backend


def fatal(msg):
    print()
    print("ERROR! " + msg)
    exit(1)


def get_args():
    parser = argparse.ArgumentParser(
        description="Expand the configuration of a C header file."
    )
    parser.add_argument(
        "--input", help="Path to the input C header file", required=True
    )
    parser.add_argument(
        "--tmp_file", help="Path to save the preprocessed output", required=True
    )
    parser.add_argument(
        "--output", help="Path to save the expanded configuration header", required=True
    )
    parser.add_argument(
        "--cc", help="C compiler used to preprocess the input", default=None
    )

    parser.add_argument(
        "--defs",
        nargs="+",
        default=[],
        help="Definitions to be passed to the preprocessor (flag -D)",
    )

    parser.add_argument(
        "--include",
        nargs="+",
        default=[],
        help="Paths to include directories for the preprocessor (flag -I)",
    )

    return parser.parse_args()


def select_backend(cc):
    """
    Returns the name of the backend to use and a callable running it
    """
    if cc_backend.is_available(cc):
        return cc_backend.NAME, lambda *args: cc_backend.preprocess(cc, *args)

    if pcpp_backend.is_available():
        return pcpp_backend.NAME, pcpp_backend.preprocess

    fatal(
        f"'{cc}' can not expand the configuration.\n"
        "Install pcpp to expand the configuration without the compiler, or set\n"
        "the CONFIG_LV_* variables manually by disabling LV_BUILD_SET_CONFIG_OPTS"
    )


def main():

    args = get_args()

    assert args.cc is not None

    name, preprocess = select_backend(args.cc)
    print(f"Expanding the configuration with {name}")

    try:
        preprocess(args.input, args.tmp_file, args.include, args.defs)
    except (subprocess.CalledProcessError, RuntimeError, OSError) as e:
        fatal(f"Preprocessing of {args.input} failed: {e}")

    try:
        lv_conf_header.generate(args.tmp_file, args.output, args.defs)
    except OSError as e:
        fatal(f"Writing {args.output} failed: {e}")

    print(f"Expanded configuration header saved to {args.output}")

    os.remove(args.tmp_file)


if __name__ == "__main__":
    main()
