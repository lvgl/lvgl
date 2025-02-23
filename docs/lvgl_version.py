"""
Utility used by `./docs/src/conf.py` and `./docs/build.py`
to fetch current LVGL version string from `lv_version.h`.
"""
import os
import re

base_path = os.path.abspath(os.path.dirname(__file__))
project_path = os.path.abspath(os.path.join(base_path, '..'))
version_source_path = os.path.join(project_path, 'lv_version.h')


def lvgl_version(version_file):
    """Build and return LVGL version string from `lv_version.h`.  Updated to be
    multi-platform compatible and resilient to changes in file in compliance
    with C macro syntax.

    :param version_file:  path to `lv_version.h`
    :return:  version string
    """
    if version_file is None:
        version_file = version_source_path

    major = ''
    minor = ''

    with open(version_file, 'r') as file:
        major_re = re.compile(r'define\s+LVGL_VERSION_MAJOR\s+(\d+)')
        minor_re = re.compile(r'define\s+LVGL_VERSION_MINOR\s+(\d+)')

        for line in file.readlines():
            # Skip if line not long enough to match.
            if len(line) < 28:
                continue

            match = major_re.search(line)
            if match is not None:
                major = match[1]
            else:
                match = minor_re.search(line)
                if match is not None:
                    minor = match[1]
                    # Exit early if we have both values.
                    if len(major) > 0 and len(minor) > 0:
                        break

    return f'{major}.{minor}'


if __name__ == '__main__':
    """Make module importable as well as run-able."""
    version_str = lvgl_version(None)
    print(version_str, end='', flush=True)
