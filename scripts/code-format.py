#!/usr/bin/env python3

import os

script_dir = os.path.realpath(__file__)
script_dir = os.path.dirname(script_dir)
cfg_file = os.path.join(script_dir, 'code-format.cfg')

print("\nFormatting demos")
os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../demos/*.c,*.cpp,*.h"')

print("\nFormatting examples")
os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../examples/*.c,*.cpp,*.h"')

print("Formatting src")
os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../src/*.c,*.cpp,*.h"')

print("\nFormatting tests")
os.system(f'astyle --options={cfg_file} --recursive "{script_dir}/../tests/*.c,*.cpp,*.h"')
