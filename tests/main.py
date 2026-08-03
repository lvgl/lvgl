#!/usr/bin/env python3

import argparse
import shutil
import subprocess
import sys
import os
import platform
from itertools import chain
from pathlib import Path

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))
lvgl_dir = os.path.dirname(lvgl_test_dir)
lvgl_script_path = os.path.join(lvgl_test_dir, "../scripts")
sys.path.append(lvgl_script_path)

from perf import perf_test_options
from LVGLImage import LVGLImage, ColorFormat, CompressMethod

lvgl_configs_dir = os.path.join(lvgl_test_dir, "configs")

# Placeholder for the defconfig fragments that depend on the host OS and
# architecture, expanded by host_defconfigs().
HOST = '@host'

# Every build configuration is a list of defconfig fragments from tests/configs,
# merged in the given order so that a later fragment overrides an earlier one.
# CMakeLists.txt derives everything else - warning flags, reference image
# directory, which libraries to look for - from the resulting configuration.
build_only_options = {
    'OPTIONS_NORMAL_8BIT': {
        'description': 'Minimal config, 8 bit color depth',
        'defconfigs': ['minimal', 'depth_8'],
    },
    'OPTIONS_16BIT': {
        'description': 'Minimal config, 16 bit color depth',
        'defconfigs': ['minimal', 'depth_16'],
    },
    # A full config at 24 bpp is not possible: the DRM, OpenGL ES and FFmpeg
    # code only supports 8, 16 and 32 bit color depth.
    'OPTIONS_24BIT': {
        'description': 'Minimal config, 24 bit color depth',
        'defconfigs': ['minimal', 'depth_24'],
    },
    'OPTIONS_FULL_32BIT': {
        'description': 'Full config, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'builtin_heap'],
    },
}

if platform.system() != 'Windows':
    build_only_options['OPTIONS_SDL'] = {
        'description': 'SDL simulator with full config, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'builtin_heap', 'sdl'],
    }

test_options = {
    'OPTIONS_TEST_SYSHEAP': {
        'description': 'Test config, system heap, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'sys_heap', 'run_tests'],
    },
    'OPTIONS_TEST_DEFHEAP': {
        'description': 'Test config, LVGL heap, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'builtin_heap', 'defheap',
                       'run_tests'],
    },
    'OPTIONS_TEST_VG_LITE': {
        'description': 'VG-Lite simulator with full config, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'sys_heap', 'run_tests',
                       'vg_lite'],
        # Cover the other side of a VG-Lite configuration switch on 32 bit
        'defconfigs_non_amd64': ['vg_lite_32bit'],
    },
    'OPTIONS_TEST_RISCV_V': {
        'description': 'RISC-V Vector emulation with full config, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'sys_heap', 'run_tests',
                       'riscv_v'],
    },
    'OPTIONS_TEST_NANOVG': {
        'description': 'NanoVG headless rendering with EGL, 32 bit color depth',
        'defconfigs': ['full', 'depth_32', HOST, 'sys_heap', 'run_tests',
                       'nanovg'],
    },
}


def get_build_config(options_name):
    if options_name in build_only_options:
        return build_only_options[options_name]
    return test_options[options_name]


def get_option_description(option_name):
    return get_build_config(option_name)['description']


def non_amd64_build():
    '''Return True when building for a non-amd64 (32 bit) target.'''
    return bool(os.environ.get('NON_AMD64_BUILD'))


def host_defconfigs():
    '''Return the defconfig fragments that depend on the host OS and CPU.

    The drivers LVGL can build differ per platform, and the 32 bit CI images do
    not ship the i386 OpenGL packages.'''
    if platform.system() == 'Windows':
        return ['windows']

    fragments = ['unix']
    if platform.system() == 'Linux':
        fragments.append('linux')
    if not non_amd64_build():
        fragments.append('opengles')
    return fragments


def target_cmake_args():
    '''Return the cmake arguments that select the target architecture and
    toolchain.

    These have to be given on the command line rather than set from a listfile,
    because the compiler and library checks LVGL runs while resolving its
    dependencies have to see them.'''
    args = []

    if platform.system() == 'Windows':
        vcpkg_root = os.environ.get('VCPKG_INSTALLATION_ROOT')
        if vcpkg_root:
            args += [
                '-DVCPKG_TARGET_TRIPLET=x64-windows',
                '-DCMAKE_TOOLCHAIN_FILE=%s' % os.path.join(
                    vcpkg_root, 'scripts', 'buildsystems', 'vcpkg.cmake'),
            ]

    if non_amd64_build():
        print('Non AMD64 target is specified')
        for flags in ('CMAKE_C_FLAGS', 'CMAKE_CXX_FLAGS',
                      'CMAKE_EXE_LINKER_FLAGS', 'CMAKE_SHARED_LINKER_FLAGS'):
            args.append('-D%s=-m32' % flags)
        # Search the i386 library paths first
        args.append('-DCMAKE_LIBRARY_PATH=/usr/lib/i386-linux-gnu')
        os.environ['PKG_CONFIG_PATH'] = os.pathsep.join(filter(None, [
            '/usr/lib/i386-linux-gnu/pkgconfig',
            os.environ.get('PKG_CONFIG_PATH')]))

    return args


def get_defconfig_paths(options_name):
    '''Return the defconfig paths for the given build options name.'''
    config = get_build_config(options_name)

    fragments = ['common']
    for fragment in config['defconfigs']:
        if fragment == HOST:
            fragments.extend(host_defconfigs())
        else:
            fragments.append(fragment)
    if non_amd64_build():
        fragments.extend(config.get('defconfigs_non_amd64', []))

    return [os.path.join(lvgl_configs_dir, '%s.defconfig' % fragment)
            for fragment in fragments]


def delete_dir_ignore_missing(dir_path):
    '''Recursively delete a directory and ignore if missing.'''
    try:
        shutil.rmtree(dir_path)
    except FileNotFoundError:
        pass


def options_abbrev(options_name):
    '''Return an abbreviated version of the option name.'''
    prefix = 'OPTIONS_'
    assert options_name.startswith(prefix)
    return options_name[len(prefix):].lower()


def get_base_build_dir(options_name):
    '''Given the build options name, return the build directory name.

    Does not return the full path to the directory - just the base name.'''
    return 'build_%s' % options_abbrev(options_name)


def get_build_dir(options_name):
    '''Given the build options name, return the build directory name.

    Returns absolute path to the build directory.'''
    global lvgl_test_dir
    return os.path.join(lvgl_test_dir, get_base_build_dir(options_name))

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

    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)
        # The tests are built as part of the LVGL project, which pulls in
        # tests/CMakeLists.txt when LV_BUILD_TESTS is set.
        cmake_args = ['cmake', '-GNinja', '-S', lvgl_dir, '-B', build_dir,
                      '-DCMAKE_BUILD_TYPE=%s' % build_type,
                      '-DLV_BUILD_TESTS=ON',
                      '-DLV_BUILD_DEFCONFIG_PATH=%s' % ';'.join(
                          get_defconfig_paths(options_name)),
                      '-DLVGL_TEST_ENABLE=%s' % (
                          'ON' if options_name in test_options else 'OFF')]
        cmake_args += target_cmake_args()
        # Use ccache as a compiler launcher when available. This dramatically
        # speeds up rebuilds across the build matrix and repeated invocations
        # (notably under emulation), and is a no-op when ccache is absent.
        if shutil.which('ccache'):
            cmake_args += ['-DCMAKE_C_COMPILER_LAUNCHER=ccache',
                           '-DCMAKE_CXX_COMPILER_LAUNCHER=ccache']
        subprocess.check_call(cmake_args)
    subprocess.check_call(['cmake', '--build', build_dir,
                           '--parallel', str(os.cpu_count())])


def run_tests(options_name, test_suite):
    '''Run the tests for the given options name.'''

    print()
    print()
    label = 'Running tests for %s' % options_abbrev(options_name)
    print('=' * len(label))
    print(label)
    print('=' * len(label), flush=True)

    os.chdir(get_build_dir(options_name))
    args = [
        'ctest',
        '--timeout', '300',
        '--parallel', str(os.cpu_count()),
        '--output-on-failure',
    ]
    if test_suite is not None:
        args.extend(["--tests-regex", test_suite])
    subprocess.check_call(args)


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
    root_dir = os.path.dirname(lvgl_test_dir)  # Get parent directory of tests (lvgl directory)
    html_report_file = 'report/index.html'
    cmd = ['gcovr', '--gcov-ignore-parse-errors', 
           '--root', root_dir, '--html-details', '--output',
           html_report_file, '--xml', 'report/coverage.xml',
           '-j', str(os.cpu_count()), '--print-summary', '--merge-mode-functions=merge-use-line-min',
           '--html-title', 'LVGL Test Coverage']

    subprocess.check_call(cmd)
    print("Done: See %s" % html_report_file, flush=True)


def generate_test_images():
    invalids = (ColorFormat.UNKNOWN,ColorFormat.RAW,ColorFormat.RAW_ALPHA)
    formats = [i for i in ColorFormat if i not in invalids]
    png_path = os.path.join(lvgl_test_dir, "test_images/pngs")
    pngs = list(Path(png_path).rglob("*.[pP][nN][gG]"))
    print(f"png files: {pngs}")

    align_options = [1, 64]

    for align in align_options:
        for compress in CompressMethod:
            compress_name = compress.name if compress != CompressMethod.NONE else "UNCOMPRESSED"
            outputs = os.path.join(lvgl_test_dir, f"test_images/stride_align{align}/{compress_name}/")
            os.makedirs(outputs, exist_ok=True)
            for fmt in formats:
                for png in pngs:
                    img = LVGLImage().from_png(png, cf=fmt, background=0xffffff)
                    img.adjust_stride(align=16)
                    output = os.path.join(outputs, f"{Path(png).stem}_{fmt.name}.bin")
                    img.to_bin(output, compress=compress)
                    output = os.path.join(outputs, f"{Path(png).stem}_{fmt.name}_{compress.name}_align{align}.c")
                    img.to_c_array(output, compress=compress)
                    print(f"converting {os.path.basename(png)}, format: {fmt.name}, compress: {compress_name}")


def clean_dir_with_filter(directory, clean_filters):
    for entry in os.listdir(directory):
        entry_path = os.path.join(directory, entry)

        if any(entry.endswith(filter) for filter in clean_filters):
            continue

        if os.path.isfile(entry_path):
            os.remove(entry_path)
        elif os.path.isdir(entry_path):
            shutil.rmtree(entry_path)


def clean_build_dirs_with_filter(build_dir, clean_filters):
    # The test executables are built in a "tests" subdirectory of the LVGL build
    # directory, so the coverage data is spread over both. Clean the nested one
    # first, then keep it while cleaning the top level.
    tests_dir = os.path.join(build_dir, 'tests')
    if os.path.isdir(tests_dir):
        clean_dir_with_filter(tests_dir, clean_filters)

    clean_dir_with_filter(build_dir, clean_filters + ['tests'])


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
                        choices=list(chain(build_only_options, test_options, perf_test_options)),
                        help='''the build option name to build or run. When
                        omitted all build configurations are used.
                        ''')
    parser.add_argument('--clean', action='store_true', default=False,
                        help='clean existing build artifacts before operation.')
    parser.add_argument('--report', action='store_true',
                        help='generate code coverage report for tests.')
    parser.add_argument('actions', nargs='*', choices=['build', 'test'],
                        help='build: compile build tests, test: compile/run executable tests.')
    parser.add_argument('--test-suite', default=None,
                        help='select test suite to run')
    parser.add_argument('--update-image', action='store_true', default=False,
                        help='Update test image using LVGLImage.py script')
    parser.add_argument('--auto-clean', action='store_true', default=False,
                        help='Automatically clean build directories')
    parser.add_argument('--keep-report', action='store_true', default=False,
                        help='Skip cleaning gcov report files when --auto-clean and --report are enabled')

    args = parser.parse_args()

    if args.update_image:
        generate_test_images()

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

    clean_build_dirs = []
    for options_name in options_to_build:
        is_test = options_name in test_options
        is_perf_test = options_name in perf_test_options
        if is_perf_test:
            perf_test_script = os.path.join(lvgl_test_dir, "perf.py")
            try:
                subprocess.check_call([perf_test_script, *(sys.argv[1:])])
            except subprocess.CalledProcessError as e:
                sys.exit(e.returncode)
            continue

        build_type = 'Debug'
        build_tests(options_name, build_type, args.clean)
        if is_test:
            try:
                run_tests(options_name, args.test_suite)
            except subprocess.CalledProcessError as e:
                sys.exit(e.returncode)

        if args.auto_clean:
            build_dir = get_build_dir(options_name)

            if args.report:
                # Keep the files that gcovr analysis depends on and delete
                # the rest to solve the storage capacity limit of GitHub CI
                clean_filters = ['CMakeFiles', '.c']
                clean_build_dirs_with_filter(build_dir, clean_filters)

                if args.keep_report:
                    # Retain the gcov report files for subsequent automated coverage analysis.
                    print(f"Keeping {build_dir} for report")
                else:
                    print(f"Append {build_dir} to clean list")
                    clean_build_dirs.append(build_dir)
            else:
                # Remove all build directories directly
                print(f"Removing {build_dir}")
                shutil.rmtree(build_dir)

    if args.report:
        generate_code_coverage_report()

    # Clean all build directories after report
    for build_dir in clean_build_dirs:
        print(f"Removing {build_dir}")
        shutil.rmtree(build_dir)
