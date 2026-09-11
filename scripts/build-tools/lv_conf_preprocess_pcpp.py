#!/usr/bin/env python3

# Expands the configuration with pcpp, a C preprocessor written in Python
# Needed for the compilers that can't emit their #define directives like MSVC

import shutil
import subprocess

NAME = "pcpp"


def build_args(input_file, output_file, include_dirs, defs):
    """
    --passthru-defines
           keep the #define directives in the output instead of consuming them
    --line-directive=
           omit the linemarkers
    """
    args = ["-o", output_file, "--passthru-defines", "--line-directive="]

    for include_path in include_dirs:
        args.append(f"-I{include_path}")

    for definition in defs:
        args.append(f"-D{definition}")

    args.append(input_file)

    return args


def import_pcpp():
    try:
        import pcpp

        return pcpp

    except ImportError:
        return None


def is_available():
    return import_pcpp() is not None or shutil.which("pcpp") is not None


def preprocess(input_file, output_file, include_dirs, defs):
    args = build_args(input_file, output_file, include_dirs, defs)

    module = import_pcpp()
    if module is not None:
        # argv[0] is the program name, pcpp expects it to be present
        preprocessor = module.CmdPreprocessor(["pcpp"] + args)
        if preprocessor.return_code:
            raise RuntimeError(
                f"pcpp failed with return code: {preprocessor.return_code}"
            )
        return

    executable = shutil.which("pcpp")
    if executable is None:
        raise RuntimeError("pcpp is not installed")

    subprocess.run([executable] + args, check=True)
