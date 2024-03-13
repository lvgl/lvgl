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

        temp_filename = rst_file + '.tmp'

        with open(temp_filename, 'w', encoding='utf-8') as temp_file:
            temp_file.write('\n:link_to_translation:`zh_CN:[中文]`\n\n')

        with open(rst_file, 'r', encoding='utf-8') as original_file, open(temp_filename, 'a', encoding='utf-8') as temp_file:
            temp_file.writelines(original_file)

        os.replace(temp_filename, rst_file)

