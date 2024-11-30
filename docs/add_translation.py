#!/usr/bin/env python3
import os

"""
Please add the translation language you want to add here, while also modifying the variable URL_BASE in _ext/link_roles.py
For example:

LANGUAGE =  ':link_to_translation:`zh_CN:[中文]`\t'  + \
            ':link_to_translation:`en:[English]`\t' + \
            '\n\n'


URL_BASE = {
    "zh_CN": "https://lvgl.100ask.net/",
    "en": "https://docs.lvgl.io/"
}
"""

LANGUAGE =  ':link_to_translation:`zh_CN:[中文]`\t'  + \
            '\n\n'



def find_files(dir_path, suffix):
    files = []

    for root, _, filenames in os.walk(dir_path):
        for filename in filenames:
            if filename.endswith(suffix):
                files.append(os.path.join(root, filename))
    return files



def exec(temp_directory):
    """
    files = find_files(temp_directory, '.rst')

    for rst_file in files:
        with open(rst_file, 'r+', encoding='utf-8') as f:
            content = f.read()
            f.seek(0, 0)
            f.write(LANGUAGE + content)
    """

    rst_file = os.path.join(temp_directory, 'index.rst')
    with open(rst_file, 'r+', encoding='utf-8') as f:
        content = f.read()
        f.seek(0, 0)
        f.write(LANGUAGE + content)
