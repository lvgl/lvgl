#!/usr/bin/env python3

import argparse
import sys
import os
import shutil
import subprocess

benchmark_configs = {
    "perf32b": {
        "description": "Benchmark config ARM (so3) Emulated - 32 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf32b:main",
        "defaults_file": "lv_conf_perf32b.defaults",
    },
    "perf64b": {
        "description": "Benchmark config ARM (so3) Emulated - 64 bit",
        "image_name": "ghcr.io/smartobjectoriented/so3-lvperf64b:main",
        "defaults_file": "lv_conf_perf64b.defaults",
    },
}

lvgl_test_dir = os.path.dirname(os.path.realpath(__file__))
lvgl_root_dir = os.path.dirname(lvgl_test_dir)


def main() -> int:
    epilog = """This program runs the LVGL demo benchmark
    In order to provide timing consistency between host computers,
    these runs are run in an ARM emulated environment inside QEMU.
    For the runtime environment, SO3 is used which is a lightweight, ARM-based 
    operating system.
    Right now, this script requires a host linux computer as we depend on
    `losetup` which is used to set up and control loop devices.
    """
    parser = argparse.ArgumentParser(
        description="Run LVGL benchmark tests.", epilog=epilog
    )
    parser.add_argument(
        "--config",
        nargs=1,
        choices=benchmark_configs.keys(),
        help="The benchmark config to use. Available configs: "
        + ", ".join(benchmark_configs.keys()),
        required=False,
    )
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
        "actions",
        nargs="*",
        choices=["generate", "run"],
        help="generate: generate the lv_conf.h from the `configs/ci/perf` folder, run: run the benchmark with the provided config",
    )

    args = parser.parse_args()

    if not args.actions:
        print("Error: At least one action must be specified")
        parser.print_help()
        return 1

    configs = []
    if args.config:
        configs = args.config
    else:
        configs = benchmark_configs.keys()

    for config in configs:
        if args.clean:
            clean(config)

        if "generate" in args.actions:
            generate_benchmark_files(config)

        if "run" in args.actions:
            run_benchmark(config, args.pull)

        if args.auto_clean:
            clean(config)

    return 0


def get_build_dir(config_name: str) -> str:
    """Given the config name, return the build directory path."""
    build_dir_name = f"build_benchmark_{config_name}"
    return os.path.join(lvgl_test_dir, build_dir_name)


def get_container_name(config_name: str) -> str:
    """
    Returns the docker container name based on the config name
    """
    return f"lv_benchmark_{config_name}"


def get_docker_volumes(config_name: str) -> list[str]:
    """
    Returns all docker volume names that should be created
    """
    return [get_build_cache_volume(config_name), get_disk_cache_volume(config_name)]


def get_build_cache_volume(config_name: str) -> str:
    """
    Returns the docker volume name for storing cmake generated files
    """
    return f"{get_container_name(config_name)}_build_cache"


def get_disk_cache_volume(config_name: str) -> str:
    """
    Returns the docker volume name for storing the virtual disks
    """
    return f"{get_container_name(config_name)}_disk_cache"


def create_dir_if_not_exists(build_dir: str) -> bool:
    """Create directory if it doesn't exist"""
    created_build_dir = False

    if os.path.exists(build_dir):
        if not os.path.isdir(build_dir):
            raise ValueError(f"{build_dir} exists but is not a directory")
    else:
        os.makedirs(build_dir, exist_ok=True)
        created_build_dir = True

    return created_build_dir


def generate_config(config_name: str) -> None:
    """Generate lv_conf.h from the defaults file for the given config"""
    print(f"Generating config for {config_name}")

    build_dir = get_build_dir(config_name)
    create_dir_if_not_exists(build_dir)

    defaults_file = benchmark_configs[config_name]["defaults_file"]
    defaults_path = os.path.join(lvgl_root_dir, "configs", "ci", "perf", defaults_file)
    template_path = os.path.join(lvgl_root_dir, "lv_conf_template.h")

    assert os.path.exists(defaults_path)
    assert template_path

    output_path = os.path.join(build_dir, "lv_conf.h")
    generate_script = os.path.join(lvgl_root_dir, "scripts", "generate_lv_conf.py")
    shutil.copy(template_path, output_path)

    cmd = [
        sys.executable,
        generate_script,
        "--template",
        template_path,
        "--config",
        output_path,
        "--defaults",
        defaults_path,
        build_dir,
    ]

    subprocess.check_call(cmd)
    print(f"Generated lv_conf.h at {output_path}")


def generate_benchmark_files(config_name: str) -> None:
    """Generate the necessary files for running benchmark inside so3"""
    print()
    print()
    label = f"Generating benchmark files for: {config_name}: {benchmark_configs[config_name]['description']}"
    print("=" * len(label))
    print(label)
    print("=" * len(label))

    build_dir = get_build_dir(config_name)
    create_dir_if_not_exists(build_dir)

    generate_config(config_name)


def clean(config_name: str) -> None:
    """Clean build directory and docker resources"""
    print(f"Cleaning resources for {config_name}")

    build_dir = get_build_dir(config_name)
    container_name = get_container_name(config_name)

    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
        print(f"Removed build directory: {build_dir}")

    subprocess.check_call(
        ["docker", "rm", "-f", container_name],
    )

    for volume in get_docker_volumes(config_name):
        subprocess.check_call(
            ["docker", "volume", "remove", "-f", volume],
        )


def run_benchmark(config_name: str, pull: bool) -> None:
    """Run the benchmark using docker"""

    def volume(src, dst):
        return ["-v", f"{src}:{dst}"]

    def so3_usr_lib(path):
        return f"/so3/usr/lib/{path}"

    so3_usr_build = "/so3/usr/build"
    persistence_dir = "/persistence"
    container_name = get_container_name(config_name)
    build_dir = get_build_dir(config_name)

    for v in get_docker_volumes(config_name):
        subprocess.check_call(["docker", "volume", "create", v])

    lvgl_src_path = os.path.join(lvgl_root_dir, "src")
    lvgl_h_path = os.path.join(lvgl_root_dir, "lvgl.h")
    lvgl_demos_path = os.path.join(lvgl_root_dir, "demos")
    lv_conf_path = os.path.join(build_dir, "lv_conf.h")
    if not os.path.exists(lv_conf_path):
        generate_config(config_name)
        assert os.path.exists(lv_conf_path)

    docker_image_name = benchmark_configs[config_name]["image_name"]

    volumes = [
        # This is necessary to create a loop device
        volume("/dev", "/dev"),
        # Replace container's lvgl source and lv_conf
        volume(lvgl_h_path, so3_usr_lib("lvgl/lvgl.h")),
        volume(lvgl_src_path, so3_usr_lib("lvgl/src")),
        volume(lvgl_demos_path, so3_usr_lib("lvgl/demos")),
        volume(lv_conf_path, so3_usr_lib("lv_conf.h")),
        # Cache build and disk folders so we don't regenerate everything in consecutive runs
        volume(get_build_cache_volume(config_name), so3_usr_build),
        volume(get_disk_cache_volume(config_name), persistence_dir),
    ]

    interactive = "-it" if sys.stdout.isatty() else "-t"
    command = [
        "docker",
        "run",
        "--rm",
        "--privileged",
        interactive,
        "--name",
        container_name,
    ]

    if pull:
        command.append("--pull=always")

    for v in volumes:
        command.extend(v)

    command.append(docker_image_name)

    print()
    print()
    label = f"Running benchmark: {config_name}: {benchmark_configs[config_name]['description']}"
    print("=" * len(label))
    print(label)
    print("=" * len(label))

    subprocess.check_call(command)


if __name__ == "__main__":
    sys.exit(main())
