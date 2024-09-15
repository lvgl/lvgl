#!/usr/bin/env python3

import os
import re
import argparse

DIR_SCRIPTS = os.path.dirname(__file__)
REPO_ROOT = os.path.join(DIR_SCRIPTS, "..")
DIR_CWD = os.getcwd()


def fatal(msg):
    print()
    print("ERROR! " + msg)
    exit(1)


def get_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description=""
                                     "Update the configuration file of your existing project based on default parameters and latest LVGL template. Eg.:\n"
                                     " python3 generate_lv_conf.py ./src \n"
                                     " python3 generate_lv_conf.py --template modules/lvgl/lv_conf_template.h ./my_config_folder"
                                     )

    parser.add_argument('--template', type=str, default=REPO_ROOT, nargs='?',
                        help='Path of "lv_conf_template.h" file or the folder containing it\n(optional, 1 folder above the python script by default)')

    parser.add_argument('--config', type=str, default=None, nargs='?',
                        help='Path of "lv_conf.h" file (optional)')

    parser.add_argument('--defaults', type=str, default=None, nargs='?',
                        help='Path of "lv_conf.defaults" file (optional)')

    parser.add_argument('target', metavar='target', type=str, default=DIR_CWD, nargs='?',
                        help='Folder containing "lv_conf.h" and "lv_conf.defaults" files\n(optional, current work folder by default)')

    args = parser.parse_args()

    if os.path.isdir(args.template):
        args.template = os.path.join(args.template, "lv_conf_template.h")

    if not args.config:
        args.config = os.path.join(args.target, "lv_conf.h")

    if not args.defaults:
        args.defaults = os.path.join(args.target, "lv_conf.defaults")

    if not os.path.exists(args.template):
        fatal(f"Template file not found at {args.template}")
    if not os.path.exists(args.config):
        fatal(f"User config file not found at {args.config}")
    if not os.path.exists(args.defaults):
        fatal(f"User defaults not found at {args.defaults}")

    return args


def parse_defaults(path: str):
    defaults = {}

    with open(path, 'r', encoding='utf-8') as file:
        for line in file.readlines():
            if len(line.strip()) == 0 or line.startswith('#'):
                continue
            groups = re.search(r'([A-Z0-9_]+)\s+(.+)', line).groups()
            defaults[groups[0]] = groups[1]

    return defaults


def generate_config(path_destination: str, path_source: str, defaults: dict):
    with open(path_source, 'r', encoding='utf-8') as f_src:
        src_lines = f_src.readlines()

    keys_used = set()
    dst_lines = []

    for src_line in src_lines:
        res = re.search(r'#define\s+([A-Z0-9_]+)\s+(.+)', src_line)
        key = res.groups()[0] if res else None

        if key in defaults.keys():
            value = defaults[key]
            pattern = r'(#define\s+[A-Z0-9_]+\s+)(.+)'
            repl = r'\g<1>' + value
            dst_line, _ = re.subn(pattern, repl, src_line)

            if not dst_line:
                fatal(f"Failed to apply key '{key}' to line '{src_line}'")

            print(f"Applying: {key} = {value}")
            keys_used.add(key)
        elif 'Set this to "1" to enable content' in src_line:
            dst_line = '#if 1 /* Enable content */'
        else:
            dst_line = src_line

        dst_lines.append(dst_line)

    if len(keys_used) != len(defaults):
        unused_keys = [k for k in defaults.keys() if k not in keys_used]
        fatal('The following keys are deprecated:\n  ' + '\n  '.join(unused_keys))

    with open(path_destination, 'w', encoding='utf-8') as f_dst:
        for dst_line in dst_lines:
            f_dst.write(dst_line)


if __name__ == '__main__':
    args = get_args()

    print("Template:", args.template)
    print("User config:", args.config)
    print("User defaults:", args.defaults)

    defaults = parse_defaults(args.defaults)
    print(f"Loaded {len(defaults)} defaults")

    generate_config(args.config, args.template, defaults)
    print()
    print('New config successfully generated!')
