#!/usr/bin/env python3

import os


project_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


def get_version():
    path = os.path.join(project_path, 'lv_version.h')

    with open(path, 'rb') as f:
        d = f.read().decode('utf-8')

    d = d.split('LVGL_VERSION_MAJOR', 1)[-1]
    major, d = d.split('\n', 1)
    d = d.split('LVGL_VERSION_MINOR', 1)[-1]
    minor, d = d.split('\n', 1)
    d = d.split('LVGL_VERSION_PATCH', 1)[-1]
    patch, d = d.split('\n', 1)
    d = d.split('LVGL_VERSION_INFO', 1)[-1]
    info, _ = d.split('\n', 1)

    ver = '{0}.{1}.{2}.{3}'.format(
        major.strip(),
        minor.strip(),
        patch.strip(),
        info.replace('"', '').strip()
    )
    return ver


if __name__ == '__main__':
    import sys

    if '--only_major_minor' in sys.argv:
        sys.stdout.write('.'.join(get_version().split('.')[:-2]) + '\n')
    else:
        sys.stdout.write(get_version() + '\n')

    sys.stdout.flush()
    sys.exit(0)
