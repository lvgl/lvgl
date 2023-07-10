#!/usr/bin/env python3

import os

print("Formatting src")
os.system('astyle --options=code-format.cfg --recursive "../src/*.c,*.h"')

print("\nFormatting demos")
os.system('astyle --options=code-format.cfg --recursive "../demos/*.c,*.h"')

print("\nFormatting examples")
os.system('astyle --options=code-format.cfg --recursive "../examples/*.c,*.h"')

print("\nFormatting tests")
os.system('astyle --options=code-format.cfg --recursive "../tests/src/test_cases/*.c"')
