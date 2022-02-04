#!/usr/bin/env python3

import os

os.system('astyle --options=code-format.cfg "../src/*.c,*.h"')
os.system('astyle --options=code-format-tests.cfg "../tests/src/test_cases/*.c"')
