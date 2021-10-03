#!/usr/bin/env python3

import argparse
import errno
import glob
import shutil
import subprocess
import sys
import os

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))

# Key values must match variable names in CMakeLists.txt.
build_only_options = {
    'OPTIONS_MINIMAL_MONOCHROME': 'Minimal config monochrome',
    'OPTIONS_NORMAL_8BIT': 'Normal config, 8 bit color depth',
    'OPTIONS_16BIT': 'Minimal config, 16 bit color depth',
    'OPTIONS_16BIT_SWAP': 'Normal config, 16 bit color depth swapped',
    'OPTIONS_FULL_32BIT': 'Full config, 32 bit color depth',
}

test_options = {
    'OPTIONS_TEST': 'Test config, 32 bit color depth',
}


def is_valid_option_name(option_name):
    return option_name in build_only_options or option_name in test_options


def get_option_description(option_name):
    if option_name in build_only_options:
        return build_only_options[option_name]
    return test_options[option_name]


def delete_dir_ignore_missing(dir_path):
    '''Recursively delete a directory and ignore if missing.'''
    try:
        shutil.rmtree(dir_path)
    except FileNotFoundError:
        pass


def generate_test_runners():
    '''Generate the test runner source code.'''
    global lvgl_test_dir
    os.chdir(lvgl_test_dir)
    delete_dir_ignore_missing('src/test_runners')
    os.mkdir('src/test_runners')

    # TODO: Intermediate files should be in the build folders, not alongside
    #       the other repo source.
    for f in glob.glob("./src/test_cases/test_*.c"):
        r = f[:-2] + "_Runner.c"
        r = r.replace("/test_cases/", "/test_runners/")
        subprocess.check_call(['ruby', 'unity/generate_test_runner.rb',
                               f, r, 'config.yml'])


def options_abbrev(options_name):
    '''Return an abbreviated version of the option name.'''
    prefix = 'OPTIONS_'
    assert options_name.startswith(prefix)
    return options_name[len(prefix):].lower()


def get_base_buid_dir(options_name):
    '''Given the build options name, return the build directory name.

    Does not return the full path to the directory - just the base name.'''
    return 'build_%s' % options_abbrev(options_name)


def get_build_dir(options_name):
    '''Given the build options name, return the build directory name.

    Returns absolute path to the build directory.'''
    global lvgl_test_dir
    return os.path.join(lvgl_test_dir, get_base_buid_dir(options_name))


def build_tests(options_name, build_type, clean):
    '''Build all tests for the specified options name.'''
    global lvgl_test_dir

    print()
    print()
    label = 'Building: %s: %s' % (options_abbrev(
        options_name), get_option_description(options_name))
    print('=' * len(label))
    print(label)
    print('=' * len(label))
    print(flush=True)

    build_dir = get_build_dir(options_name)
    if clean:
        delete_dir_ignore_missing(build_dir)

    os.chdir(lvgl_test_dir)
    created_build_dir = False
    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)
        created_build_dir = True
    os.chdir(build_dir)
    if created_build_dir:
        subprocess.check_call(['cmake', '-DCMAKE_BUILD_TYPE=%s' % build_type,
                               '-D%s=1' % options_name, '..'])
    subprocess.check_call(['cmake', '--build', build_dir,
                           '--parallel', str(os.cpu_count())])


def run_tests(options_name):
    '''Run the tests for the given options name.'''

    print()
    print()
    label = 'Running tests for %s' % options_abbrev(options_name)
    print('=' * len(label))
    print(label)
    print('=' * len(label), flush=True)

    os.chdir(get_build_dir(options_name))
    subprocess.check_call(
        ['ctest', '--parallel', str(os.cpu_count()), '--output-on-failure'])


def generate_code_coverage_report():
    '''Produce code coverage test reports for the test execution.'''
    global lvgl_test_dir

    print()
    print()
    label = 'Generating code coverage reports'
    print('=' * len(label))
    print(label)
    print('=' * len(label))
    print(flush=True)

    os.chdir(lvgl_test_dir)
    delete_dir_ignore_missing('report')
    os.mkdir('report')
    root_dir = os.pardir
    html_report_file = 'report/index.html'
    cmd = ['gcovr', '--root', root_dir, '--html-details', '--output',
           html_report_file, '--xml', 'report/coverage.xml',
           '-j', str(os.cpu_count()), '--print-summary',
           '--html-title', 'LVGL Test Coverage']
    for d in ('.*\\bexamples/.*', '\\bsrc/test_.*', '\\bsrc/lv_test.*', '\\bunity\\b'):
        cmd.extend(['--exclude', d])

    subprocess.check_call(cmd)
    print("Done: See %s" % html_report_file, flush=True)


if __name__ == "__main__":
    epilog = '''This program builds and optionally runs the LVGL test programs.
    There are two types of LVGL tests: "build", and "test". The build-only
    tests, as their name suggests, only verify that the program successfully
    compiles and links (with various build options). There are also a set of
    tests that execute to verify correct LVGL library behavior.
    '''
    parser = argparse.ArgumentParser(
        description='Build and/or run LVGL tests.', epilog=epilog)
    parser.add_argument('--build-options', nargs=1,
                        help='''the build option name to build or run. When 
                        omitted all build configurations are used.
                        ''')
    parser.add_argument('--clean', action='store_true', default=False,
                        help='clean existing build artifacts before operation.')
    parser.add_argument('--report', action='store_true',
                        help='generate code coverage report for tests.')
    parser.add_argument('actions', nargs='*', choices=['build', 'test'],
                        help='build: compile build tests, test: compile/run executable tests.')

    args = parser.parse_args()

    if args.build_options:
        options_to_build = args.build_options
    else:
        if 'build' in args.actions:
            if 'test' in args.actions:
                options_to_build = {**build_only_options, **test_options}
            else:
                options_to_build = build_only_options
        else:
            options_to_build = test_options

    for opt in options_to_build:
        if not is_valid_option_name(opt):
            print('Invalid build option "%s"' % opt, file=sys.stderr)
            sys.exit(errno.EINVAL)

    generate_test_runners()

    for options_name in options_to_build:
        is_test = options_name in test_options
        build_type = 'Debug'
        build_tests(options_name, build_type, args.clean)
        if is_test:
            try:
                run_tests(options_name)
            except subprocess.CalledProcessError as e:
                sys.exit(e.returncode)

    if args.report:
        generate_code_coverage_report()
