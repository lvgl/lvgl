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
dest_config = os.path.join(base_path, 'lv_conf.h')
src_config = os.path.abspath(os.path.join(
    base_path,
    '..',
    'lv_conf_template.h'
))
disabled_option_re = re.compile(r'^\s*#define\s+(LV_(?:USE|FONT)_\w+)\s+(\b0\b)')

leave_disabled_list = [
    'LV_USE_PROFILER',
    'LV_USE_DRAW_ARM2D_SYNC',
    'LV_USE_NATIVE_HELIUM_ASM',
]


def run(output_cfg_path=None):
    global dest_config
    enable_content_macro_processed = False
    os.chdir(base_path)

    if output_cfg_path is not None:
        dest_config = output_cfg_path

    with open(src_config, 'r') as f:
        data = f.read()

    lines = data.split('\n')

    for i, line in enumerate(lines):
        if not enable_content_macro_processed:
            if line.startswith('#if 0'):
                line = line.replace('#if 0', '#if 1')
                lines[i] = line
                enable_content_macro_processed = True
        else:
            match = disabled_option_re.search(line)
            if match:
                # Except for these...
                if match[1] in leave_disabled_list:
                    continue
                else:
                    # ...replace '0' with '1' without altering any other part of line.
                    # Set `j` to index where '0' was found.
                    j = match.regs[2][0]
                    # Surgically insert '1' in place of '0'.  Strings are immutable.
                    line = line[:j] + '1' + line[j + 1:]
                    lines[i] = line

    data = '\n'.join(lines)

    with open(dest_config, 'w') as f:
        f.write(data)


def cleanup():
    if os.path.exists(dest_config):
        os.remove(dest_config)


if __name__ == '__main__':
    """Make module importable as well as run-able."""
    run()
