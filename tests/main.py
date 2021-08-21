#!/usr/bin/env python3

import glob
import shutil
import subprocess
import sys
import os

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))
num_test_case_failures = 0

# Key values must match variable names in CMakeLists.txt.
options = {
    'OPTIONS_MINIMAL_MONOCHROME': 'Minimal config monochrome',
    'OPTIONS_NORMAL_8BIT': 'Normal config, 8 bit color depth',
    'OPTIONS_16BIT': 'Minimal config, 16 bit color depth',
    'OPTIONS_16BIT_SWAP': 'Normal config, 16 bit color depth swapped',
    'OPTIONS_FULL_32BIT': 'Full config, 32 bit color depth',
    'OPTIONS_TEST': 'Test config, 32 bit color depth',
}

# Most test configurations only build - this is the one that also
# executes.
run_tests_option_name = 'OPTIONS_TEST'


def delete_dir_ignore_missing(dir_path):
    '''Recursively delete a directory and ignore if missing.'''
    try:
        shutil.rmtree(dir_path)
    except FileNotFoundError:
        pass


def generate_test_runners():
    '''Generate the test runner source code.

    Returns a list of test names.'''
    global lvgl_test_dir
    os.chdir(lvgl_test_dir)
    delete_dir_ignore_missing('src/test_runners')
    os.mkdir('src/test_runners')

    # TODO: Intermediate files should be in the build folders, not alongside
    #       the other repo source.
    test_names = []
    for f in glob.glob("./src/test_cases/test_*.c"):
        test_names.append(os.path.splitext(os.path.basename(f))[0])
        r = f[:-2] + "_Runner.c"
        r = r.replace("/test_cases/", "/test_runners/")
        subprocess.check_call(['ruby', 'unity/generate_test_runner.rb',
                               f, r, 'config.yml'])
    return test_names


def options_abbrev(options_name):
    '''Return an abbreviated version of the option name.'''
    prefix = 'OPTIONS_'
    assert options_name.startswith(prefix)
    return options_name[len(prefix):].lower()


def get_build_dir(options_name):
    '''Given the build options name, return the build directory name.

    Does not return the full path to the directory - just the base name.'''
    return 'build_%s' % options_abbrev(options_name)


def delete_build_dir(options_name):
    '''Recursively delete the build directory for the given options name.'''
    global lvgl_test_dir
    build_dir = os.path.join(lvgl_test_dir, get_build_dir(options_name))
    delete_dir_ignore_missing(build_dir)


def build_tests(options_name, build_type):
    '''Build all tests for the specified options name.'''
    global options, lvgl_test_dir, test_noclean

    print()
    print()
    label = 'Building: %s: %s' % (options_abbrev(
        options_name), options[options_name])
    print('=' * len(label))
    print(label)
    print('=' * len(label))
    print("", flush=True)

    if not test_noclean:
        delete_build_dir(options_name)

    os.chdir(lvgl_test_dir)
    build_dir = get_build_dir(options_name)
    created_build_dir = False
    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)
        created_build_dir = True
    os.chdir(build_dir)
    if created_build_dir:
        subprocess.check_call(['cmake', '-DCMAKE_BUILD_TYPE=%s' % build_type,
                               '-D%s=1' % options_name, '..'])
    subprocess.check_call(['cmake', '--build', os.path.join(lvgl_test_dir, build_dir),
                           '--parallel', str(os.cpu_count())])


def run_tests(options_name, test_names):
    '''Run the tests for the given options name.'''
    global num_test_case_failures
    relative_bd = get_build_dir(options_name)
    abs_bd = os.path.join(lvgl_test_dir, relative_bd)
    for test_name in test_names:

        print()
        print()
        label = 'Running: %s' % os.path.join(
            options_abbrev(options_name), test_name)
        print('=' * len(label))
        print(label)
        print('=' * len(label))

        try:
            os.chdir(lvgl_test_dir)
            subprocess.check_call([os.path.join(abs_bd, test_name)])
        except subprocess.CalledProcessError as e:
            num_test_case_failures += 1


def generate_code_coverage_report():
    '''Produce code coverage test reports for the test execution.'''
    global lvgl_test_dir

    print()
    print()
    label = 'Generating code coverage reports'
    print('=' * len(label))
    print(label)
    print('=' * len(label))
    print("", flush=True)

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


run_test_only = "test" in sys.argv
generate_gcov_report = "report" in sys.argv
test_noclean = "noclean" in sys.argv

test_names = generate_test_runners()

for options_name in options.keys():
    is_test = options_name == run_tests_option_name
    build_type = 'Release' if is_test else 'Debug'
    if is_test or not run_test_only:
        build_tests(options_name, build_type)
    if options_name == run_tests_option_name:
        run_tests(options_name, test_names)

if generate_gcov_report:
    generate_code_coverage_report()

print()
if num_test_case_failures:
    print('There were %d test case failures.' %
          num_test_case_failures, file=sys.stderr)
else:
    print('All test cases passed.')
sys.exit(num_test_case_failures)
