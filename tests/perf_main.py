from pathlib import Path
import os
import shutil
import subprocess
import os

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))


def lvgl_test_src(name):
    return os.path.join(lvgl_test_dir, "src", name)


LVGL_TEST_FILES = [lvgl_test_src("lv_test_init.c"), lvgl_test_src("lv_test_init.h")]


perf_test_options = {
    "OPTIONS_TEST_PERF_32B": {
        "description": "Perf test config ARM (so3) Emulated - 32 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf32b:main",
    },
    "OPTIONS_TEST_PERF_64B": {
        "description": "Perf test config ARM (so3) Emulated - 64 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf64b:main",
    },
}


def options_abbrev(options_name):
    """Return an abbreviated version of the option name."""
    prefix = "OPTIONS_"
    assert options_name.startswith(prefix)
    return options_name[len(prefix) :].lower()


def get_base_build_dir(options_name):
    """Given the build options name, return the build directory name.

    Does not return the full path to the directory - just the base name."""
    return "build_%s" % options_abbrev(options_name)


def create_build_dir(build_dir):
    created_build_dir = False

    if os.path.exists(build_dir):
        if not os.path.isdir(build_dir):
            raise ValueError(f"{build_dir} exists but is not a directory")
    else:
        os.mkdir(build_dir)
        created_build_dir = True

    return created_build_dir


def find_c_files(directory):
    c_files = []

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".c"):
                c_files.append(os.path.join(root, file))

    return c_files


def get_build_dir(options_name):
    """Given the build options name, return the build directory name.

    Returns absolute path to the build directory."""
    global lvgl_test_dir
    return os.path.join(lvgl_test_dir, get_base_build_dir(options_name))


def generate_so3_init_commands(runners: list[tuple[str, str]], path: str):
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
        delimiter = 50 * "="
        output.append(f"echo {delimiter}")
        output.append(f"echo Running {name_without_extension}")
        output.append(f"echo {delimiter}")
        output.append(f"run {name_without_extension}.elf")

    output.append("exit")
    with open(path, "w") as f:
        f.write("\n".join(output))


def generate_perf_test_cmakelists(runners: list[tuple[str, str]], path: str):
    """
    Generates the CMakeLists.txt that will be mounted in `usr/src/test_src/CMakeLists.txt`
    This file simply declares every runner as a different executable and links the necessary
    libraries
    """
    output = []
    files_to_include = [
        os.path.basename(file) for file in LVGL_TEST_FILES if file.endswith(".c")
    ]

    for runner, test_case in runners:
        runner_name_without_extension = Path(runner).stem
        runner_elf_file = f"{runner_name_without_extension}.elf"
        output.append(
            f"add_executable({runner_elf_file} {runner_name_without_extension}.c {test_case} {" ".join(files_to_include)})"
        )
        output.append(f"target_link_libraries({runner_elf_file} c lvgl unity)")
        output.append(
            f"target_compile_definitions({runner_elf_file} PRIVATE LV_BUILD_TEST LV_BUILD_TEST_PERF)",
        )
        output.append(
            f"target_include_directories({runner_elf_file} PRIVATE ${{CMAKE_CURRENT_SOURCE_DIR}}/../unity)"
        )

    with open(path, "w") as f:
        f.write("\n".join(output))


def generate_unity_cmakelists(path: str):

    content_lines = [
        "add_library(unity STATIC unity.c)",
        "target_compile_definitions(unity PUBLIC LV_BUILD_TEST LV_BUILD_TEST_PERF UNITY_INCLUDE_DOUBLE UNITY_OUTPUT_COLOR)",
    ]

    with open(path, "w") as f:
        f.write("\n".join(content_lines))


def generate_main_so3_cmakelists(path: str):
    """
    Generates the main CMakeLists.txt that will be mounted in `usr/src/CMakeLists.txt`
    We need to keep the `init` program as it's the program that will be
    responsible for launching every runner
    We also add a new subdirectory `test_src` that will be the folder containing
    every test runner, and necessary sources for building those runners
    (except the lvgl source code)
    """
    content_lines = [
        "add_subdirectory(unity)",
        "add_executable(init.elf init.c)",
        "target_link_libraries(init.elf c)",
        "add_subdirectory(test_src)",
    ]

    with open(path, "w") as f:
        f.write("\n".join(content_lines))


def generate_perf_test_runners(options_name):
    """
    Generates every necessary file for running tests inside so3
    Everything is built inside a docker container so we need to prepare
    every file we need to mount before mounting and running the container
    """

    build_dir = get_build_dir(options_name)
    test_src_dir = os.path.join(build_dir, "test_src")
    unity_src_dir = os.path.join(lvgl_test_dir, "unity")
    unity_target_dir = os.path.join(build_dir, "unity")

    create_build_dir(build_dir)

    if os.path.exists(test_src_dir):
        shutil.rmtree(test_src_dir)

    create_build_dir(test_src_dir)

    unity_generate_runner_script_path = os.path.join(
        lvgl_test_dir, "unity", "generate_test_runner.rb"
    )
    # Get the necessary files in order to generate the runners
    # This includes the test cases and the unity config
    perf_test_src = find_c_files(
        os.path.join(lvgl_test_dir, "src", "test_cases", "perf")
    )
    unity_config_path = os.path.join(lvgl_test_dir, "config.yml")
    runners = []
    for src_path in perf_test_src:
        runner_file_name = Path(src_path).stem + "_Runner.c"
        test_case_name = os.path.basename(src_path)
        test_case_path = os.path.join(test_src_dir, test_case_name)
        runner_path = os.path.join(test_src_dir, runner_file_name)

        subprocess.check_call(
            [
                unity_generate_runner_script_path,
                src_path,
                runner_path,
                unity_config_path,
            ]
        )

        # Copy the original test case so its included inside the container
        shutil.copy(src_path, test_case_path)
        runners.append((runner_file_name, test_case_name))

    print(f"Copying Unity {unity_src_dir} -> {unity_target_dir}")
    shutil.copytree(unity_src_dir, unity_target_dir, dirs_exist_ok=True)

    for src in LVGL_TEST_FILES:
        dst = os.path.join(test_src_dir, os.path.basename(src))
        shutil.copy(src, dst)

    generate_main_so3_cmakelists(os.path.join(build_dir, "CMakeLists.txt"))
    generate_perf_test_cmakelists(runners, os.path.join(test_src_dir, "CMakeLists.txt"))
    generate_so3_init_commands(runners, os.path.join(build_dir, "commands.ini"))
    generate_unity_cmakelists(os.path.join(unity_target_dir, "CMakeLists.txt"))


def run_tests(options_name, config_name):
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

    build_dir = get_build_dir(options_name)

    unity_dir = os.path.join(build_dir, "unity")
    test_src_dir = os.path.join(build_dir, "test_src")
    main_cmakelists = os.path.join(build_dir, "CMakeLists.txt")
    lvgl_src_dir = os.path.join(lvgl_test_dir, "..", "src")
    lv_conf_path = os.path.join(lvgl_test_dir, "src", config_name)
    lvgl_h_path = os.path.join(lvgl_test_dir, "..", "lvgl.h")

    commands_ini_path = os.path.join(build_dir, "commands.ini")

    build_cache_dir = os.path.join(build_dir, "build")
    virtual_disk_cache_dir = os.path.join(build_dir, "persistence")

    command = ["docker", "run", "-it", "--rm"]

    # The so3-lvperf images need to be run with the --privileged flag
    # and /dev mounted as a virtual disk is created inside the docker container
    command.append("--privileged")
    command.extend(volume("/dev", "/dev"))

    # Replace container's lvgl source and lv_conf
    command.extend(volume(lvgl_src_dir, so3_usr_lib("lvgl/src")))
    command.extend(volume(lvgl_src_dir, so3_usr_lib("lvgl/demos")))
    command.extend(volume(lvgl_src_dir, so3_usr_lib("lvgl/examples")))
    command.extend(volume(lv_conf_path, so3_usr_lib("lv_conf.h")))

    command.extend(volume(lvgl_h_path, "/so3/usr/lvgl.h"))

    # Mount the necessary files. Everything will be built inside the container
    command.extend(volume(test_src_dir, so3_usr_src("test_src")))
    command.extend(volume(unity_dir, so3_usr_src("unity")))
    command.extend(volume(main_cmakelists, so3_usr_src("CMakeLists.txt")))
    command.extend(volume(commands_ini_path, so3_usr_out("commands.ini")))

    # Mount the cmake build output
    command.extend(volume(build_cache_dir, so3_usr_build))
    command.extend(volume(virtual_disk_cache_dir, persistence_dir))

    # Temporary
    command.extend(volume("/home/andre/dev/so3/usr/lib/libc", so3_usr_lib("libc")))
    #

    command.extend(["--name", f"lv_test_{options_name}"])
    command.append(perf_test_options[options_name]["image_name"])

    subprocess.check_call(command)


def main():
    generate_perf_test_runners("OPTIONS_TEST_PERF_32B")
    run_tests("OPTIONS_TEST_PERF_32B", "lv_test_perf_conf.h")


if __name__ == "__main__":
    main()
