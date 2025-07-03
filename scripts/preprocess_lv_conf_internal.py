#!/usr/bin/env python3

#
# Preprocess the lv_conf_internal.h to generate a header file
# containing the evaluated definitions. This output will be used to
# generate the cmake variables
#
# Author: David TRUAN (david.truan@edgemtech.ch)
# Author: Erik Tagirov (erik.tagirov@edgemtech.ch)
#

import sys
import subprocess
import os
import argparse
import re
import shutil
import importlib.util


def get_args():
    parser = argparse.ArgumentParser(description="Preprocess a C header file and remove indentation.")
    parser.add_argument("--input", help="Path to the input C header file", required=True)
    parser.add_argument("--tmp_file", help="Path to save the preprocessed output", required=True)
    parser.add_argument("--output", help="Path to save the cleaned output with removed indentation", required=True)
    parser.add_argument("--workfolder", help="Path used to create a python environment", required=True)

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


# Read the temporary files and return an array of lines
def read_lines(tmp_file):

    try:
        with open(tmp_file, "r") as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Failed to pre-process file: {e}")
        exit(1)

    return lines

def remove_indentation(lines):

    clean_lines = []

    try:
        for line in lines:
            stripped = line.lstrip()

            # Remove extra spaces after #
            if stripped.startswith("#"):
                stripped = re.sub(r"^#\s+", "#", stripped)

            clean_lines.append(stripped)


    except Exception as e:
        print(f"Error during indentation removal: {e}")
        exit(1)

    return clean_lines

# This is required - to avoid include errors when Kconfig is used and
# LVGL is installed on the system -  i.e when lvgl.h is used as a system include
def add_include_guards(lines):

    lines.insert(0, "#define LV_CONF_H\n\n")
    lines.insert(0, "#ifndef LV_CONF_H\n")
    lines.append("#endif /* END LV_CONF_H */\n")
    return lines

def init_venv(venv_path):
    """
    Creates a virtual env named .venv in the `workfolder` directory
    It is usually set to the path of the build directory.
    If the .venv already exists it enters the venv
    Returns the path to the venv
    """
    try:
        if os.path.exists(venv_path) == False:
            # Create venv
            subprocess.check_call([sys.executable, "-m", "venv", venv_path])

        # Enter venv
        venv_path = os.path.join(venv_path, ".venv")
        subprocess.check_call([sys.executable, "-m", "venv", venv_path])

    except subprocess.CalledProcessError as e:
        print(f"Error during the setup of python venv: {e}")
        exit(1)

    return venv_path

def install_pcpp_in_venv(venv_path):
    """
    Install pcpp a python implementation of the C pre-processor
    On success - Returns the path to pcpp, None on failure
    """

    if sys.platform == "win32":
        venv_pip = os.path.join(venv_path, "Scripts", "pip.exe")
        venv_pcpp = os.path.join(venv_path, "Scripts", "pcpp.exe")
    else:
        venv_pip = os.path.join(venv_path, "bin", "pip")
        venv_pcpp = os.path.join(venv_path, "bin", "pcpp")

    if os.path.exists(venv_pcpp) == False:
        # Install pcpp
        try:
            subprocess.check_call([venv_pip, "install", "pcpp"])

        except subprocess.CalledProcessError as e:
            print(f"Failed to install PCPP: {e}")
            return None
    else:
        print("PCPP is already installed in venv")


    return venv_pcpp

def main():

    args = get_args()

    # Check if PCPP is already present on the system
    # if it's not - create a python venv inside the workfolder directory
    # and install it there
    pcpp_path = shutil.which("pcpp")

    if pcpp_path:
        pcpp_exe = pcpp_path
        print(f"Found PCPP: {pcpp_exe}")
    else:
        print("Failed to locate pcpp - installing it")
        venv_path = init_venv(args.workfolder)
        pcpp_exe = install_pcpp_in_venv(venv_path)
        if pcpp_exe is None:
            exit(1)

    preprocess_file(pcpp_exe, args.input, args.tmp_file, args.output, args.include, args.defs)

    lines = read_lines(args.tmp_file)
    lines = remove_indentation(lines)
    lines = add_include_guards(lines)

    # Write the resulting output header file with include guards and no indentation
    try:
        with open(args.output, "w") as f:
            f.writelines(lines)
    except Exception as e:
        print(f"Writing resulting file failed: {e}")
        exit(1)

    print(f"Expanded configuration header saved to {args.output}")

    os.remove(args.tmp_file)
    print(f"Temporary preprocessed file {args.tmp_file} removed.")

if __name__ == "__main__":
    main()
