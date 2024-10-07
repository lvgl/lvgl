#!/usr/bin/env python3

import os
import re
import argparse


def get_arg():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description=""
                                     "Apply the specified version to affected source files. Eg.:\n"
                                     " python3 update_version.py 9.1.2-dev\n"
                                     " python3 update_version.py 9.2.0"
                                     )
    parser.add_argument('version', metavar='version', type=str,
                        help='The version to apply')

    return parser.parse_args()


class Version:
    RE_PATTERN = r"(\d+)\.(\d+)\.(\d+)(-[\w\d]+)?"

    def __init__(self, user_input: str):

        if not re.match(r'^' + self.RE_PATTERN + r'$', user_input):
            raise Exception(f"Invalid version format: {user_input}")

        groups = re.search(self.RE_PATTERN, user_input).groups()

        self.major = groups[0]
        self.minor = groups[1]
        self.patch = groups[2]
        self.info = groups[3].lstrip('-') if groups[3] else ""

        self.is_release = len(self.info) == 0
        self.as_string = user_input

    def __str__(self):
        return self.as_string


class RepoFileVersionReplacer:
    DIR_SCRIPTS = os.path.dirname(__file__)
    DIR_REPO_ROOT = os.path.join(DIR_SCRIPTS, "..")

    def __init__(self, relative_path_segments: list[str], expected_occurrences: int):
        self.path_relative = os.path.join(*relative_path_segments)
        self.path = os.path.join(self.DIR_REPO_ROOT, self.path_relative)
        self.expected_occurrences = expected_occurrences

    def applyVersionToLine(self, line: str, version: Version) -> str | None:
        return None

    def applyVersion(self, version: Version):
        with open(self.path, 'r', encoding='utf-8') as file:
            lines = file.readlines()

        occurrences = 0
        for i, line in enumerate(lines):
            line_with_version = self.applyVersionToLine(line, version)
            if line_with_version:
                lines[i] = line_with_version
                occurrences += 1

        # not perfect, but will catch obvious pitfalls
        if occurrences != self.expected_occurrences:
            raise Exception(f"Bad lines in {self.path_relative}")

        with open(self.path, 'w', encoding='utf-8') as file:
            file.writelines(lines)


class PrefixReplacer(RepoFileVersionReplacer):

    def __init__(self, relative_path_segments: list[str], prefix: str, expected_occurrences=1):
        super().__init__(relative_path_segments, expected_occurrences)
        self.prefix = prefix

    def applyVersionToLine(self, line: str, version: Version):
        pattern = r'(' + re.escape(self.prefix) + ')' + Version.RE_PATTERN
        repl = r'\g<1>' + str(version)
        replaced, n = re.subn(pattern, repl, line)
        return replaced if n > 0 else None


class MacroReplacer(RepoFileVersionReplacer):
    def __init__(self, relative_path_segments: list[str]):
        super().__init__(relative_path_segments, 4)

    def applyVersionToLine(self, line: str, version: Version):
        targets = {
            'LVGL_VERSION_MAJOR': version.major,
            'LVGL_VERSION_MINOR': version.minor,
            'LVGL_VERSION_PATCH': version.patch,
            'LVGL_VERSION_INFO': version.info,
        }

        for key, val in targets.items():
            pattern = self.getPattern(key)
            repl = self.getReplacement(val)
            replaced, n = re.subn(pattern, repl, line)
            if n > 0:
                return replaced

        return None

    def getPattern(self, key: str):
        return r'(^#define ' + key + r' +).+'

    def getReplacement(self, val: str):
        if not val.isnumeric():
            val = f'"{val}"'

        return r'\g<1>' + val


class CmakeReplacer(MacroReplacer):
    def getPattern(self, key: str):
        return r'(^set\(' + key + r' +")([^"]*)(.+)'

    def getReplacement(self, val: str):
        return r'\g<1>' + val + r'\g<3>'

class KconfigReplacer(RepoFileVersionReplacer):
    """Replace version info in Kconfig file"""

    def __init__(self, relative_path_segments: list[str]):
        super().__init__(relative_path_segments, 3)

    def applyVersionToLine(self, line: str, version: Version):
        targets = {
            'LVGL_VERSION_MAJOR': version.major,
            'LVGL_VERSION_MINOR': version.minor,
            'LVGL_VERSION_PATCH': version.patch,
        }

        for key, val in targets.items():
            pattern = self.getPattern(key)
            repl = self.getReplacement(val)
            replaced, n = re.subn(pattern, repl, line)
            if n > 0:
                return replaced

        return None
    def getPattern(self, key: str):
        # Match the version fields in Kconfig file
        return rf'(^\s+default\s+)(\d+) # ({key})'

    def getReplacement(self, val: str):
        # Replace the version value
        return r'\g<1>' + val + r' # \g<3>'


if __name__ == '__main__':
    args = get_arg()

    version = Version(args.version)
    print(f"Applying version {version} to:")

    targets = [
        MacroReplacer(['lv_version.h']),
        CmakeReplacer(['env_support', 'cmake', 'version.cmake']),
        PrefixReplacer(['lv_conf_template.h'], 'Configuration file for v'),
        KconfigReplacer(['Kconfig']),
    ]

    if version.is_release:
        targets.extend([
            PrefixReplacer(['library.json'], '"version": "'),
            PrefixReplacer(['library.properties'], 'version='),
            PrefixReplacer(['Kconfig'], 'Kconfig file for LVGL v'),
        ])

    for target in targets:
        print(f"  - {target.path_relative}")
        target.applyVersion(version)
