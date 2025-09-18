#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path
import os
import shutil
import subprocess
import os
from typing import Optional

perf_test_options = {
    "OPTIONS_TEST_PERF_32B": {
        "description": "Perf test config ARM (so3) Emulated - 32 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf32b:main",
        "config": "lv_test_perf_conf.h",
    },
    "OPTIONS_TEST_PERF_64B": {
        "description": "Perf test config ARM (so3) Emulated - 64 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf64b:main",
        "config": "lv_test_perf_conf.h",
    },
}

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))


def main() -> bool:
    epilog = """This program runs LVGL perfomance tests
    In order to provide timing consitency between host computers,
    these runs are run in an ARM emulated environnement inside QEMU.
    For the runtime environnement, SO3 is used which is a lightweight, ARM-based 
    operating system.
    Right now, this script requires a host linux computer as we depend on
    `losetup` which is used to set up and control loop devices.
    """
    parser = argparse.ArgumentParser(
        description="Run LVGL performance tests.", epilog=epilog
    )
    parser.add_argument(
        "--build-options",
        nargs=1,
        choices=perf_test_options.keys(),
        help="""the perf test option name to run. When
                omitted all build configurations are used.
             """,
    )
    parser.add_argument("--test-suite", default=None, help="Select test suite to run")
    parser.add_argument(
        "--pull",
        action="store_true",
        default=False,
        help="Pull latest images from registry before running tests",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        default=False,
        help="Clean existing build artifacts before operation",
    )
    parser.add_argument(
        "--auto-clean",
        action="store_true",
        default=False,
        help="Automatically clean build directories",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        default=False,
        help="Launch QEMU in debug mode allowing you to connect to gdb",
    )
    parser.add_argument(
        "--debug-port",
        default=5555,
        help="Port used to connect to QEMU in debug mode",
    )
    parser.add_argument(
        "actions",
        nargs="*",
        choices=["build", "generate", "test"],
        help="build | generate: generates run files dependencies, test: run performance tests.",
    )

    args = parser.parse_args()

    options = []
    if args.build_options:
        options = args.build_options
    else:
        options = perf_test_options.keys()

    # Since test suites are run inside a virtual space, debugging them is tricky
    # because every test suite will be loaded into memory at the same virtual address
    # this means that when you try to set a breakpoint (e.g: in `main`), gdb will actually
    # hit that breakpoint for EACH test_suite that runs, which can be very confusing
    # Instead, only allow debugging if a test_suite is selected as we won't encounter that problem
    if args.debug and not args.test_suite:
        print("Please provide the test suite you want to debug")
        exit(1)

    is_error = False
    for option_name in options:
        if any(action in args.actions for action in ("generate", "build", "test")):
            if args.clean:
                clean(option_name)
            generate_files(option_name, args.test_suite)

        if "test" in args.actions:
            ret = run_tests(
                option_name,
                "lv_test_perf_conf.h",
                args.pull,
                args.debug,
                args.debug_port,
                args.test_suite,
            )
            is_error = is_error or not ret

        if args.auto_clean:
            clean(option_name)

    return is_error


def write_lines_to_file(path: str, lines: list[str]) -> None:
    """
    Writes a list of strings to a file, each on a separate line
    """
    with open(path, "w") as f:
        f.write("\n".join(lines))


def lvgl_test_src(name: str) -> str:
    """
    Gets the absolute path of a file from the lvgl `tests/src` folder
    """
    return os.path.join(lvgl_test_dir, "src", name)


LVGL_TEST_FILES = [lvgl_test_src("lv_test_init.c"), lvgl_test_src("lv_test_init.h")]


def options_abbrev(options_name: str) -> str:
    """Return an abbreviated version of the option name."""
    prefix = "OPTIONS_"
    assert options_name.startswith(prefix)
    return options_name[len(prefix) :].lower()


def get_base_build_dir(options_name: str) -> str:
    """Given the build options name, return the build directory name.

    Does not return the full path to the directory - just the base name."""
    return "build_%s" % options_abbrev(options_name)


def create_dir(build_dir: str) -> bool:
    created_build_dir = False

    if os.path.exists(build_dir):
        if not os.path.isdir(build_dir):
            raise ValueError(f"{build_dir} exists but is not a directory")
    else:
        os.mkdir(build_dir)
        created_build_dir = True

    return created_build_dir


def find_c_files(directory: str) -> list[str]:
    c_files = []

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".c"):
                c_files.append(os.path.join(root, file))

    return c_files


def get_container_name(options_name: str) -> str:
    """
    Returns the docker container name based on the options name
    """
    return f"lv_perf_test_{options_name}"


def get_docker_volumes(options_name: str) -> list[str]:
    """
    Returns all docker volume names that should be created in order to run the images
    This function is useful so we can easily loop through the necessary docker volumes in order
    to create or delete them eg:

    ```python
    for v in get_docker_volumes(options_name):
        subprocess.check_call(["docker", "volume", "create", v])
        subprocess.check_call(["docker", "volume", "remove", v])
    ```
    """
    return [get_build_cache_volume(options_name), get_disk_cache_volume(options_name)]


def get_build_cache_volume(options_name: str) -> str:
    """
    Returns the docker volume name for storing cmake generated files

    We use a docker volume here instead of a plain directory because the user that launches the script
    will not own the generated files as they're generated inside the docker container
    Using a docker volume allows us to delete it without having to run delete the volume inside this script
    without admin permissions by removing the docker volume:

    ```python
    subprocess.check_call(["docker", "volume", "remove", get_build_cache_volume(options_name)])
    ```
    """
    return f"{get_container_name(options_name)}_build_cache"


def get_disk_cache_volume(options_name: str) -> str:
    """
    Returns the docker volume name for storing the virtual disks generated inside the docker volume

    We use a docker volume here instead of a directory because the user that launches the script
    will not own the generated files as they're generated inside the docker container
    Using a docker volume allows us to delete it without having to run delete the volume inside this script
    without admin permissions by removing the docker volume:

    ```python
    subprocess.check_call(["docker", "volume", "remove", get_disk_cache_volume(options_name)])
    ```
    """
    return f"{get_container_name(options_name)}_disk_cache"


def get_build_dir(options_name: str) -> str:
    """Given the build options name, return the build directory name.

    Returns absolute path to the build directory."""
    return os.path.join(lvgl_test_dir, get_base_build_dir(options_name))


def generate_so3_init_commands(runners: list[tuple[str, str]], path: str) -> None:
    """
    Generates the `commands.ini` file that will be mounted in `usr/out/commands.ini` replacing
    the default `commands.ini` used by so3.
    A `commands.ini` file declares some specific commands the `init` program should run
    sequentially. We use this feature to run every perf test.
    An example that runs tests A and B before exiting looks like this :
    ```
    run test_A.elf
    run test_B.elf
    exit
    ```
    """

    output = []
    for runner, _ in runners:
        name_without_extension = Path(runner).stem
        label = f"Running {name_without_extension}"
        delimiter = "=" * len(label)
        output.append(f"echo {delimiter}")
        output.append(f"echo {label}")
        output.append(f"echo {delimiter}")
        output.append(f"run {name_without_extension}.elf")

    output.append("exit")
    write_lines_to_file(path, output)


def generate_perf_test_cmakelists(runners: list[tuple[str, str]], path: str) -> None:
    """
    Generates the CMakeLists.txt that will be mounted in `usr/src/test_src/CMakeLists.txt`
    This file simply declares every runner as a different executable and links the necessary
    libraries
    """
    output = []
    files_to_include = " ".join(
        os.path.basename(file) for file in LVGL_TEST_FILES if file.endswith(".c")
    )

    for runner, test_case in runners:
        runner_name_without_extension = Path(runner).stem
        runner_elf_file = f"{runner_name_without_extension}.elf"
        output.append(
            f"add_executable({runner_elf_file} {runner_name_without_extension}.c {test_case} {files_to_include})"
        )
        output.append(f"target_link_libraries({runner_elf_file} c lvgl unity)")
        output.append(
            f"target_compile_definitions({runner_elf_file} PRIVATE LV_BUILD_TEST LV_BUILD_TEST_PERF)",
        )
        output.append(
            f"target_include_directories({runner_elf_file} PRIVATE ${{CMAKE_CURRENT_SOURCE_DIR}}/../unity)"
        )
        output.append(f"target_compile_options({runner_elf_file} PRIVATE -ggdb)")

    write_lines_to_file(path, output)


def copy_unity(target_folder: str) -> None:
    """
    Copies the unity source files to `target_folder`
    """

    print(f"Generating {target_folder}")

    unity_src_dir = os.path.join(lvgl_test_dir, "unity")
    shutil.copytree(unity_src_dir, target_folder, dirs_exist_ok=True)


def copy_lvgl_test_files(target_folder: str) -> None:
    """
    Copies the lvgl test source files `target_folder`
    """

    print(f"Generating {target_folder}")
    for src in LVGL_TEST_FILES:
        dst = os.path.join(target_folder, os.path.basename(src))
        shutil.copy(src, dst)


def generate_unity_cmakelists(path: str) -> None:
    """
    Generates a CMakeLists.txt for the unity framework
    """

    print(f"Generating {path}")
    content_lines = [
        # Create a static library
        "add_library(unity STATIC unity.c)",
        # Add the necessary compile definitions in order to build the library correctly
        # Setting these to PUBLIC allows other apps that link with this library to use the same compile definitions
        # Here we enable LV_BUILD_TEST and LV_BUILD_TEST_PERF so we enable the file contents
        # UNITY_INCLUDE_DOUBLE enables macros like `TEST_ASSERT_LESS_OR_EQUAL_DOUBLE`
        # UNITY_OUTPUT_COLOR enables coloring the `OK` and the `FAIL` output of the tests
        "target_compile_definitions(unity PUBLIC LV_BUILD_TEST LV_BUILD_TEST_PERF UNITY_INCLUDE_DOUBLE UNITY_OUTPUT_COLOR)",
    ]

    write_lines_to_file(path, content_lines)


def generate_so3_usr_cmakelists(path: str) -> None:
    """
    Generates the main CMakeLists.txt that will be mounted in `usr/src/CMakeLists.txt`
    We need to keep the `init` program as it's the program that will be
    responsible for launching every runner
    We also add a new subdirectory `test_src` that will be the folder containing
    every test runner, and necessary sources for building those runners
    (except the lvgl source code)
    """

    print(f"Generating {path}")
    content_lines = [
        "add_subdirectory(unity)",
        "add_executable(init.elf init.c)",
        "target_link_libraries(init.elf c)",
        "add_subdirectory(test_src)",
    ]

    write_lines_to_file(path, content_lines)


def generate_test_runners(
    output_folder: str, test_suite: Optional[str]
) -> list[tuple[str, str]]:
    """
    Generates performance test runners by creating a runner for each `.c` file
    located in the `test_cases/perf` directory.

    If `test_suite` is provided, only `.c` files matching the suite will be used.
    Otherwise, all `.c` files in the directory are included.

    Each generated runner, along with its corresponding `.c` file, will be saved
    in the specified `output_folder`.
    """

    runner_generator_script = os.path.join(
        lvgl_test_dir, "unity", "generate_test_runner.rb"
    )

    # Get the necessary files in order to generate the runners
    # This includes the test cases and the unity config
    test_cases = find_c_files(os.path.join(lvgl_test_dir, "src", "test_cases_perf"))
    unity_config_path = os.path.join(lvgl_test_dir, "config.yml")

    runners = []

    print("Generating test case runners")

    for src_test_case_path in test_cases:
        current_test_suite = Path(src_test_case_path).stem

        if test_suite and test_suite not in current_test_suite:
            continue

        print(f"\t{current_test_suite}")

        runner_file_name = current_test_suite + "_runner.c"
        test_case_file_name = os.path.basename(src_test_case_path)

        test_case_path = os.path.join(output_folder, test_case_file_name)
        runner_path = os.path.join(output_folder, runner_file_name)

        subprocess.check_call(
            [
                runner_generator_script,
                src_test_case_path,
                runner_path,
                unity_config_path,
            ]
        )

        # Copy the original test case as still need them in the build process
        shutil.copy(src_test_case_path, test_case_path)
        # Store a tuple of runner - test case so we can generate the cmakelists later
        runners.append((runner_file_name, test_case_file_name))

    return runners


def generate_files(options_name, test_suite):
    """
    Generates every necessary file for running tests inside so3
    Everything is built inside a docker container so we need to prepare
    every file we need to mount before mounting and running the container
    """
    print()
    print()
    label = "Generating: %s: %s" % (
        options_abbrev(options_name),
        perf_test_options[options_name]["description"],
    )
    print("=" * len(label))
    print(label)
    print("=" * len(label))

    options_build_dir = get_build_dir(options_name)
    generated_unity_dir = os.path.join(options_build_dir, "unity")
    generated_test_src_dir = os.path.join(options_build_dir, "test_src")

    create_dir(options_build_dir)
    create_dir(generated_test_src_dir)

    runners = generate_test_runners(generated_test_src_dir, test_suite)

    # Copy lvgl common test files
    copy_lvgl_test_files(generated_test_src_dir)
    # Copy Unity framework
    copy_unity(generated_unity_dir)

    # Generate necessary cmakelists
    generate_unity_cmakelists(os.path.join(generated_unity_dir, "CMakeLists.txt"))
    generate_perf_test_cmakelists(
        runners, os.path.join(generated_test_src_dir, "CMakeLists.txt")
    )
    generate_so3_usr_cmakelists(os.path.join(options_build_dir, "CMakeLists.txt"))
    generate_so3_init_commands(runners, os.path.join(options_build_dir, "commands.ini"))


def clean(options_name: str) -> None:

    options_build_dir = get_build_dir(options_name)
    container_name = get_container_name(options_name)
    if os.path.exists(options_build_dir):
        shutil.rmtree(options_build_dir)

    subprocess.check_call(["docker", "rm", "-f", container_name])
    for v in get_docker_volumes(options_name):
        subprocess.check_call(["docker", "volume", "remove", "-f", v])


def check_for_success(container_name):
    """
    There's no support for return codes when running qemu so we manually check the docker
    logs to see if any case failed
    """
    stdout = subprocess.getoutput(f"docker logs {container_name}")

    for line in stdout.splitlines():
        if "FAIL" in line.strip():
            return False

    return True


def run_tests(
    options_name: str,
    lv_conf_name: str,
    pull: bool,
    debug: bool,
    debug_port: int,
    test_suite: Optional[str],
) -> bool:
    """
    Runs the tests by running the docker image associated with `options_name`
    while mounting the correct volumes from the previous generated files
    """

    def volume(src, dst):
        return ["-v", f"{src}:{dst}"]

    def so3_usr_src(path):
        return f"/so3/usr/src/{path}"

    def so3_usr_lib(path):
        return f"/so3/usr/lib/{path}"

    def so3_usr_out(path):
        return f"/so3/usr/out/{path}"

    so3_usr_build = f"/so3/usr/build"
    persistence_dir = f"/persistence"
    container_name = get_container_name(options_name)
    build_dir = get_build_dir(options_name)

    for v in get_docker_volumes(options_name):
        subprocess.check_call(["docker", "volume", "create", v])

    unity_dir = os.path.join(build_dir, "unity")
    test_src_dir = os.path.join(build_dir, "test_src")
    main_cmakelists = os.path.join(build_dir, "CMakeLists.txt")
    lvgl_src_dir = os.path.join(lvgl_test_dir, "..", "src")
    lv_conf_path = os.path.join(lvgl_test_dir, "src", lv_conf_name)
    lvgl_h_path = os.path.join(lvgl_test_dir, "..", "lvgl.h")
    lvgl_private_h_path = os.path.join(lvgl_test_dir, "..", "lvgl_private.h")
    commands_ini_path = os.path.join(build_dir, "commands.ini")
    docker_image_name = perf_test_options[options_name]["image_name"]

    volumes = [
        # This is necessary in order to create a loop device
        # It is also the reason we only support linux for now.
        volume("/dev", "/dev"),
        # Replace container's lvgl source and lv_conf
        volume(lvgl_src_dir, so3_usr_lib("lvgl/src")),
        volume(lv_conf_path, so3_usr_lib("lv_conf.h")),
        volume(lvgl_h_path, so3_usr_lib("lvgl/lvgl.h")),
        volume(lvgl_private_h_path, so3_usr_lib("lvgl/lvgl_private.h")),
        # We also need to add the current "lvgl.h" and mount it in the correct path
        # As there's a `#include "../../lvgl.h"` in the `unity_support.h` file
        volume(lvgl_h_path, "/so3/usr/lvgl.h"),
        # Mount the test sources (test cases and runners)
        volume(test_src_dir, so3_usr_src("test_src")),
        # Mount the test framework
        volume(unity_dir, so3_usr_src("unity")),
        # Modify the default so3 CMakeLists and commands.ini
        volume(main_cmakelists, so3_usr_src("CMakeLists.txt")),
        volume(commands_ini_path, so3_usr_out("commands.ini")),
        # Cache build and disk folders so we don't regenerate everything in consecutive runs
        volume(get_build_cache_volume(options_name), so3_usr_build),
        volume(get_disk_cache_volume(options_name), persistence_dir),
    ]

    interactive = "-it" if sys.stdout.isatty() else "-t"
    command = ["docker", "run", "--privileged", "--name", container_name, interactive]

    print()
    print()
    label = "Launching: %s: %s" % (
        options_abbrev(options_name),
        perf_test_options[options_name]["description"],
    )
    print("=" * len(label))
    print(label)
    print("=" * len(label))

    if pull:
        command.append("--pull=always")

    if debug:
        command.append("-e")
        command.append("SO3_USR_DEBUG=1")
        command.append("-p")
        command.append(f"{debug_port}:1234")

    for v in volumes:
        command.extend(v)

    command.append(docker_image_name)

    try:
        subprocess.check_call(command)
    except subprocess.CalledProcessError:
        subprocess.check_call(["docker", "rm", "-f", container_name])
        return False
    success = check_for_success(container_name)

    # We can't use the `docker run --rm` syntax because we need access to the docker container
    # after it exits in order to check for the success status of the run
    subprocess.check_call(["docker", "rm", "-f", container_name])
    return success


if __name__ == "__main__":
    sys.exit(main())
