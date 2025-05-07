"""
Create lv_conf.h in same directory as this file
from ../lv_conf_template.h that has:

1.  all its #define LV_USE... 0-or-1 options set to 1
     (except for LV_USER_PROFILER),
2.  all its #define LV_FONT... 0-or-1 options set to 1,
3.  its #if 0 directive set to #if 1.
"""
import os
import sys
import re

base_path = os.path.dirname(__file__)
dst_config = os.path.join(base_path, 'lv_conf.h')
src_config = os.path.abspath(os.path.join(
    base_path,
    '..',
    'lv_conf_template.h'
))
disabled_option_re = re.compile(r'^\s*#define\s+\w+\s+(\b0\b)')


def run(c_path=None):
    global dst_config
    os.chdir(base_path)

    if c_path is not None:
        dst_config = c_path

    with open(src_config, 'r') as f:
        data = f.read()

    lines = data.split('\n')

    for i, line in enumerate(lines):
        if 'LV_USE_PROFILER' in line:
            continue

        # These 2 fonts have been deprecated in favor of
        # LV_FONT_SOURCE_HAN_SANS_SC_14_CJK and
        # LV_FONT_SOURCE_HAN_SANS_SC_16_CJK.
        if 'LV_FONT_SIMSUN_14_CJK' in line:
            continue

        if 'LV_FONT_SIMSUN_16_CJK' in line:
            continue

        if 'LV_USE' in line or ('LV_FONT' in line and '#define' in line):
            match = disabled_option_re.search(line)
            if match:
                # Replace '0' with '1' without altering any other part of line.
                # Set `j` to index where '0' was found.
                j = match.regs[1][0]
                # Surgically insert '1' in place of '0'.
                lines[i] = line[:j] + '1' + line[j + 1:]
        elif line.startswith('#if 0'):
            line = line.replace('#if 0', '#if 1')
            lines[i] = line

    data = '\n'.join(lines)

    with open(dst_config, 'w') as f:
        f.write(data)


def cleanup():
    if os.path.exists(dst_config):
        os.remove(dst_config)


if __name__ == '__main__':
    """Make module importable as well as run-able."""
    run()
