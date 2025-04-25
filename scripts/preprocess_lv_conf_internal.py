#!/usr/bin/env python3

#
# Preprocess the lv_conf_internal.h to generate a header file
# containing the evaluated definitions. This output will be used to
# generate the cmake variables
#
# Author: David TRUAN (david.truan@edgemtech.ch)
#

import sys
import subprocess
import os
import argparse
import re

def get_args():
    parser = argparse.ArgumentParser(description="Preprocess a C header file and remove indentation.")
    parser.add_argument("--input", help="Path to the input C header file", required=True)
    parser.add_argument("--tmp_file", help="Path to save the preprocessed output", required=True)
    parser.add_argument("--output", help="Path to save the cleaned output with removed indentation", required=True)
    parser.add_argument("--workfolder", help="Path used to create a python environnement", required=True)

    parser.add_argument(
        "--defs",
        nargs='+',
        default=[],
        help="Definitions to be added to pcpp (flag -D)"
    )

    parser.add_argument(
        "--include",
        nargs='+',
        default=[],
        help="Paths to include directories for the preprocessor (flag -I)"
    )


    return parser.parse_args()

def preprocess_file(pcpp_exe, input_file, tmp_file, output_file, include_dirs, defs):

    try:
        pcpp_command = [pcpp_exe, "-o", tmp_file, "--passthru-defines", "--line-directive=",  input_file]

        for include_path in include_dirs:
            pcpp_command.append(f"-I{include_path}")

        for definition in defs:
            pcpp_command.append(f"-D{definition}")

        subprocess.run(pcpp_command, check=True)
        print(f"Preprocessing completed. Output saved to {tmp_file}")

    except subprocess.CalledProcessError as e:
        print(f"Error during preprocessing: {e}")
        exit(1)


def remove_indentation(tmp_file, output_file):

    try:
        with open(tmp_file, "r") as f:
            lines = f.readlines()

        clean_lines = []
        for line in lines:
            stripped = line.lstrip()

            # Remove extra spaces after #
            if stripped.startswith("#"):
                stripped = re.sub(r"^#\s+", "#", stripped)

            clean_lines.append(stripped)

        with open(output_file, "w") as f:
            f.writelines(clean_lines)

        print(f"Indentation removed. Cleaned output saved to {output_file}")

        os.remove(tmp_file)
        print(f"Temporary preprocessed file {tmp_file} removed.")
    except Exception as e:
        print(f"Error during indentation removal: {e}")
        exit(1)

def install_pcpp_in_venv(workfolder:str) -> str:
    """
    Creates a virtual env named .venv inside `workfolder`
    and installs every dependecy inside `dependencies`
    Returns the path to pcpp
    """
    venv_path = os.path.join(workfolder, ".venv")
    try:
        subprocess.check_call([sys.executable, "-m", "venv", venv_path])
        
        if sys.platform == "win32":
            venv_pip = os.path.join(venv_path, "Scripts", "pip.exe")
            venv_pcpp = os.path.join(venv_path, "Scripts", "pcpp.exe")
        else:
            venv_pip = os.path.join(venv_path, "bin", "pip")
            venv_pcpp = os.path.join(venv_path, "bin", "pcpp")
        
        subprocess.check_call([venv_pip, "install", "pcpp"])

    except subprocess.CalledProcessError as e:
        print(f"Error setting up environnement: {e}")
        sys.exit(1)

    return venv_pcpp

def main():

    args = get_args()

    pcpp_exe = install_pcpp_in_venv(args.workfolder)
    preprocess_file(pcpp_exe, args.input, args.tmp_file, args.output, args.include, args.defs)

    remove_indentation(args.tmp_file, args.output)

if __name__ == "__main__":
    main()
