#!/usr/bin/env python3

import os

os.system('astyle --options=code-format.cfg --recursive "../src/*.c,*.h"')
os.system('astyle --options=code-format.cfg --recursive "../demos/*.c,*.h"')
os.system('astyle --options=code-format.cfg --recursive "../examples/*.c,*.h"')
os.system('astyle --options=code-format.cfg --recursive "../tests/src/test_cases/*.c"')
