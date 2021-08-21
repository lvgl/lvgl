#!/usr/bin/env python3

import os
import re
import shutil
import subprocess

test_dir = os.path.dirname(os.path.realpath(__file__))
lvgl_dir = os.path.abspath(os.path.join(test_dir, os.pardir))
lvgl_dir_name = 'lvgl'  # LVGL subdirectory (base name) in lvgl_dir.
lvgl_parent_dir = os.path.abspath(os.path.join(lvgl_dir, os.pardir))

lv_conf_path = os.path.join(lvgl_dir, 'tests/src/lv_test_conf.h')
base_defines = ['-DLV_CONF_PATH="%s"' % lv_conf_path, '-DLV_BUILD_TEST']

def maybe_quote(obj):
    if type(obj) == str:
        return '"%s"' % obj
    return obj


def zip_defines(defines):
    return ['-D%s=%s' % (key, maybe_quote(defines[key])) for key in defines]


def build(defines):
    global base_defines
    optimization = ['-O3', '-g0']
    d_all = base_defines + zip_defines(defines)

    cmd_env = os.environ.copy()
    cmd_env['BIN'] = 'test.bin'
    cmd_env['MAINSRC'] = 'src/lv_test_main.c'
    cmd_env['LVGL_DIR'] = lvgl_parent_dir
    cmd_env['LVGL_DIR_NAME'] = lvgl_dir_name
    cmd_env['DEFINES'] = ' '.join(d_all)
    cmd_env['OPTIMIZATION'] = ' '.join(optimization)

    print("")
    print("Build")
    print("-----------------------", flush=True)
    # -s makes it silence
    subprocess.check_call(['make', '-s', '--jobs=%d' % os.cpu_count()], env=cmd_env)

    print("")
    print("Run")
    print("-----------------------", flush=True)
    subprocess.check_call("./test.bin")


def build_test(defines, test_name):
    global base_defines
    optimization = ['-g0']

    print("")
    print("")
    print("~~~~~~~~~~~~~~~~~~~~~~~~")
    print(re.search("/[a-z_]*$", test_name).group(0)[1:])
    print("~~~~~~~~~~~~~~~~~~~~~~~~", flush=True)

    d_all = base_defines + zip_defines(defines)

    test_file_name = test_name + ".c"
    test_file_runner_name = test_name + "_Runner.c"
    test_file_runner_name = test_file_runner_name.replace(
        "/test_cases/", "/test_runners/")
    cmd_env = os.environ.copy()
    cmd_env['BIN'] = 'test.bin'
    cmd_env['MAINSRC'] = test_file_name
    cmd_env['TEST_SRC'] = test_file_runner_name
    cmd_env['LVGL_DIR'] = lvgl_parent_dir
    cmd_env['LVGL_DIR_NAME'] = lvgl_dir_name
    cmd_env['DEFINES'] = ' '.join(d_all)
    cmd_env['OPTIMIZATION'] = ' '.join(optimization)
    extra_csrcs = [
        'unity/unity.c', 'unity/unity_support.c',
        'src/test_fonts/font_1.c', 'src/test_fonts/font_2.c',
        'src/test_fonts/font_3.c'
    ]
    cmd_env['EXTRA_CSRCS'] = ' '.join(extra_csrcs)

    print("")
    print("Build")
    print("-----------------------", flush=True)
    # -s makes it silence
    subprocess.check_call(['make', '-s', '--jobs=%d' % os.cpu_count()], env=cmd_env)

    print("")
    print("Run")
    print("-----------------------")
    subprocess.check_call('./test.bin')


def clean():
    print("")
    print("Clean")
    print("-----------------------", flush=True)

    cmd_env = os.environ.copy()
    cmd_env['LVGL_DIR'] = lvgl_parent_dir
    cmd_env['LVGL_DIR_NAME'] = lvgl_dir_name
    subprocess.check_call(['make', 'clean'], env=cmd_env)
    try:
        os.remove('test.bin')
    except FileNotFoundError:
        pass
