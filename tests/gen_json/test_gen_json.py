# Basic test to see if the API jeson generator is able to run without any errors
# This test doesn't not check to make sure the output is correct. It is for the
# sole purpose of making sure it completes.

import os
import sys

BASE_PATH = os.path.abspath(os.path.dirname(__file__))
SCRIPT_PATH = os.path.join(BASE_PATH, "..", "..", "scripts", "gen_json", "gen_json.py")

OUTPUT_FILE = os.path.join(BASE_PATH, "lvgl.json")

cmd = f'{sys.executable} "{SCRIPT_PATH}" --output-path "{BASE_PATH}"'
print("running test")
print(cmd)

result = os.system(cmd)
if result != 0:
    print()
    sys.stderr.write("TEST FAILED!!\n\n")
    sys.stderr.flush()
    sys.exit(result)


if not os.path.exists(OUTPUT_FILE):
    print()
    sys.stderr.write(f'"{OUTPUT_FILE}" was not found.\n')
    sys.stderr.write("TEST FAILED!!\n\n")
    sys.stderr.flush()
    sys.exit(-500)

try:
    os.remove(OUTPUT_FILE)
except:  # NOQA
    pass

# Check C bitmask enum expressions independently of LVGL's current values.
sys.path.insert(0, os.path.dirname(SCRIPT_PATH))
from pycparser import c_parser  # NOQA
import pycparser_monkeypatch  # NOQA

ast = c_parser.CParser().parse('''
typedef enum {
    TEST_FLAG_1 = (1u << 4),
    TEST_FLAG_2 = (1u << 8),
    TEST_FLAG_COMBINED = (TEST_FLAG_1 | TEST_FLAG_2)
} test_flag_t;
''')
ast.setup_docs(True, '', '', '')
values = {
    item['name']: int(item['value'], 16)
    for item in ast.to_dict()['enums'][0]['members']
}
assert values == {
    'TEST_FLAG_1': 0x10,
    'TEST_FLAG_2': 0x100,
    'TEST_FLAG_COMBINED': 0x110,
}

print()
print("TEST PASSED!")
