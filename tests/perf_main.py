from pathlib import Path
import os
import subprocess

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))


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


def generate_so3_init_commands(runners: list[str], path: str):

    output = ""
    for runner in runners:
        name_without_extension = Path(runner).stem
        output += f"run {name_without_extension}.elf"

    with open(path, "w") as f:
        f.write(output)


def generate_perf_test_cmakelists(runners: list[str], path: str):
    output = ""
    for runner in runners:
        name_without_extension = Path(runner).stem
        output += (
            f"add_executable({name_without_extension}.elf {name_without_extension}.c)\n"
        )

    output += "\n"

    for runner in runners:
        name_without_extension = Path(runner).stem
        output += f"target_link_libraries({name_without_extension}.elf c lvgl)\n"

    with open(path, "w") as f:
        f.write(output)


def generate_perf_test_runners(options_name):

    build_dir = get_build_dir(options_name)
    create_build_dir(build_dir)

    perf_test_src = find_c_files(os.path.join(lvgl_test_dir, "perf", "src"))
    unity_generate_runner_script_path = os.path.join(
        lvgl_test_dir, "unity", "generate_test_runner.rb"
    )
    runners = []
    for src_path in perf_test_src:
        runner_file_name = Path(src_path).stem + "_Runner.c"
        runner_path = os.path.join(build_dir, runner_file_name)
        subprocess.check_call(
            [unity_generate_runner_script_path, src_path, runner_path]
        )
        runners.append(runner_path)

    generate_perf_test_cmakelists(runners, os.path.join(build_dir, "CMakeLists.txt"))
    generate_so3_init_commands(runners, os.path.join(build_dir, "init_commands.txt"))


def main():
    generate_perf_test_runners("OPTIONS_PERF_64B")


if __name__ == "__main__":
    main()
