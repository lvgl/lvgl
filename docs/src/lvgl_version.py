"""
Utility used by `./docs/src/conf.py` and `./docs/build.py`
to fetch current LVGL version string from `lv_version.h`.
"""
import os
import re


def lvgl_version(version_file):
    """Build and return LVGL version string from `lv_version.h`.  Updated to be
    multi-platform compatible and resilient to changes in file in compliance
    with C macro syntax.

    :param version_file:  path to `lv_version.h`
    :return:  version string
    """
    major = ''
    minor = ''

    if version_file is not None:
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
    base_dir = os.path.abspath(os.path.dirname(__file__))
    project_dir = os.path.abspath(os.path.join(base_dir, '..', '..'))
    version_file = os.path.join(project_dir, "lv_version.h")
    version_str = lvgl_version(version_file)
    print(version_str, end='', flush=True)
