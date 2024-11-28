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

    for arg in args:
        if arg == "demos":
            include_demos = True
        elif arg == "examples":
            include_examples = True
        elif arg == "src":
            include_src = True
        elif arg == "tests":
            include_tests = True
        else:
            print(f'Argument [{arg}] not recognized.')
            print('Usage:')
            print('  python code-format.py [dir [dir ...]]')
            print('  where: dir can be demos, examples, src or tests.')
            exit(1)

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
    print("\nFormatting src")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../src/*.c,*.cpp,*.h"')

if include_tests:
    print("\nFormatting tests")
    os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../tests/*.c,*.cpp,*.h"')
