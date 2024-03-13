#!/usr/bin/env python3
import os


def find_files(dir_path, suffix):
    files = []

    for root, _, filenames in os.walk(dir_path):
        for filename in filenames:
            if filename.endswith(suffix):
                files.append(os.path.join(root, filename))
    return files



def exec(temp_directory):
    files = find_files(temp_directory, '.rst')

    for rst_file in files:
        with open(rst_file, 'r+', encoding='utf-8') as f:
            content = f.read()
            f.seek(0, 0)
            f.write(':link_to_translation:`zh_CN:[中文]`\n\n' + content)
