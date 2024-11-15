#!/usr/bin/env python3

import os
import sys

# Argument enhancement:  to only run `astyle` on a specified directory, to
# only include changed source code, these arguments have been added.  If
# run with no arguments, all the normal directories are examined as before:
# - /demos/
# - /examples/
# - /src/
# - /tests/
#
# Args:
# If ANY args are specified, ONLY run `astyle` on the specified directories.
# Any combination can be specified.
include_demos = True
include_examples = True
include_src = True
include_tests = True

args = sys.argv[1:]

# Have any args been specified?
if args:
    include_demos = False
    include_examples = False
    include_src = False
    include_tests = False

    if "demos" in args:
        include_demos = True
    if "examples" in args:
        include_examples = True
    if "src" in args:
        include_src = True
    if "tests" in args:
        include_tests = True

script_dir = os.path.realpath(__file__)
script_dir = os.path.dirname(script_dir)
cfg_file = os.path.join(script_dir, 'code-format.cfg')

if include_demos:
    print("\nFormatting demos")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../demos/*.c,*.cpp,*.h"')

if include_examples:
    print("\nFormatting examples")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../examples/*.c,*.cpp,*.h"')

if include_src:
    print("Formatting src")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../src/*.c,*.cpp,*.h"')

if include_tests:
    print("\nFormatting tests")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../tests/*.c,*.cpp,*.h"')
