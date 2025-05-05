"""
Create lv_conf.h in same directory as this file
from ../lv_conf_template.h that has:

1.  all its #define LV_USE... 0-or-1 options set to 1
     (except for LV_USER_PROFILER),
2.  all its #define LV_FONT... 0-or-1 options set to 1,
3.  its #if 0 directive set to #if 1.
"""
import os

base_path = os.path.dirname(__file__)
dst_config = os.path.join(base_path, 'lv_conf.h')
src_config = os.path.abspath(os.path.join(
    base_path,
    '..',
    'lv_conf_template.h'
))


def run(c_path=None):
    global dst_config

    if c_path is not None:
        dst_config = c_path

    with open(src_config, 'r') as f:
        data = f.read()

    data = data.split('\n')

    for i, line in enumerate(data):
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
            line = [item for item in line.split(' ') if item]

            for j, item in enumerate(line):
                if item == '0':
                    line[j] = '1'

            line = ' '.join(line)
            data[i] = line
        elif line.startswith('#if 0'):
            line = line.replace('#if 0', '#if 1')
            data[i] = line

    data = '\n'.join(data)

    with open(dst_config, 'w') as f:
        f.write(data)


def cleanup():
    if os.path.exists(dst_config):
        os.remove(dst_config)
