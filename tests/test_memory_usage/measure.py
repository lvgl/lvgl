#!/usr/bin/env python3
"""Build the memory-usage test configurations and report their flash and RAM usage."""

import argparse
import glob
import json
import os
import shutil
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

TARGETS = {
    "arm-cortex-m4": "arm-none-eabi-gcc",
    "arm-cortex-m0plus": "arm-none-eabi-gcc",
    "xtensa-esp32": "xtensa-esp32-elf-gcc",
    "riscv32-esp32c3": "riscv32-esp-elf-gcc",
}

CONFIGS = ["standard", "rgb565", "minimal"]


def run(cmd, **kwargs):
    return subprocess.run(cmd, check=True, capture_output=True, text=True, **kwargs)


def binutil(build_dir, name):
    """Derive a binutils path from the compiler CMake actually used for this build."""
    cache = os.path.join(build_dir, "CMakeCache.txt")
    with open(cache) as f:
        for line in f:
            if line.startswith("CMAKE_C_COMPILER:"):
                compiler = line.split("=", 1)[1].strip()
                break
        else:
            raise RuntimeError(f"no CMAKE_C_COMPILER in {cache}")

    if not compiler.endswith("gcc"):
        raise RuntimeError(f"unexpected compiler name: {compiler}")
    return compiler[: -len("gcc")] + name


def symbol_sizes(build_dir, elf):
    """Per-symbol sizes from the linked image, so a regression can be localised."""
    out = run(
        [binutil(build_dir, "nm"), "--print-size", "--size-sort", "--radix=d", elf]
    ).stdout
    sizes = {}
    for line in out.splitlines():
        fields = line.split()
        if len(fields) != 4:
            continue
        _, size, _, name = fields
        sizes[name] = sizes.get(name, 0) + int(size)
    return sizes


def library_size(build_dir):
    """Size of liblvgl.a with nothing dead-stripped."""

    matches = glob.glob(os.path.join(build_dir, "**", "liblvgl.a"), recursive=True)
    if not matches:
        raise SystemExit(f"no liblvgl.a under {build_dir}")
    out = run([binutil(build_dir, "size"), "-t", matches[0]]).stdout
    text, data, bss = (int(v) for v in out.splitlines()[-1].split()[:3])
    return {"text": text, "data": data, "bss": bss, "size": text + data}


def measure(target, config, build_root):
    build_dir = os.path.join(build_root, f"{target}-{config}")
    shutil.rmtree(build_dir, ignore_errors=True)

    toolchain = os.path.join(SCRIPT_DIR, "toolchain", f"{target}.cmake")
    run(
        [
            "cmake",
            "-S",
            os.path.join(SCRIPT_DIR, config),
            "-B",
            build_dir,
            "-GNinja",
            f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
        ]
    )
    run(["cmake", "--build", build_dir])

    elf = os.path.join(build_dir, "main")

    out = run([binutil(build_dir, "size"), elf]).stdout
    text, data, bss = (int(v) for v in out.splitlines()[-1].split()[:3])
    sizes = {
        "text": text,
        "data": data,
        "bss": bss,
        "flash": text + data,
        "ram": data + bss,
        "library": library_size(build_dir),
    }
    return sizes, symbol_sizes(build_dir, elf)


def measured_commit():
    """The commit these numbers describe, so a stale baseline is visible in the report."""
    try:
        return run(["git", "-C", SCRIPT_DIR, "rev-parse", "HEAD"]).stdout.strip()
    except (subprocess.CalledProcessError, OSError):
        return ""


def write_results(out, results, all_symbols):
    out_dir = os.path.dirname(os.path.abspath(out))
    os.makedirs(out_dir, exist_ok=True)
    with open(out, "w") as f:
        json.dump(results, f, indent=2)

    commit = measured_commit()
    if commit:
        with open(os.path.join(out_dir, "commit.txt"), "w") as f:
            f.write(commit + "\n")

    # Kept beside the totals rather than in them: the totals stay readable, and report.py
    # only needs these for the rows that actually moved.
    symbols_dir = os.path.join(out_dir, "symbols")
    os.makedirs(symbols_dir, exist_ok=True)
    for (target, config), symbols in all_symbols.items():
        with open(os.path.join(symbols_dir, f"{target}-{config}.json"), "w") as f:
            json.dump(symbols, f, indent=2, sort_keys=True)
    return symbols_dir


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "--out", default="size-results.json", help="where to write the results"
    )
    parser.add_argument(
        "--build-root",
        default=os.path.join(SCRIPT_DIR, "build"),
        help="directory to build in",
    )
    parser.add_argument(
        "--targets", nargs="+", default=list(TARGETS), choices=list(TARGETS)
    )
    parser.add_argument("--configs", nargs="+", default=CONFIGS, choices=CONFIGS)
    parser.add_argument(
        "--skip-missing",
        action="store_true",
        help="skip targets whose compiler is not on PATH",
    )
    args = parser.parse_args()

    results = {}
    all_symbols = {}
    for target in args.targets:
        if shutil.which(TARGETS[target]) is None:
            if args.skip_missing:
                print(
                    f"skipping {target}: {TARGETS[target]} not found", file=sys.stderr
                )
                continue
            raise SystemExit(
                f"{TARGETS[target]} not found on PATH (needed for {target})"
            )

        for config in args.configs:
            try:
                sizes, symbols = measure(target, config, args.build_root)
            except subprocess.CalledProcessError as e:
                print(e.stdout, file=sys.stderr)
                print(e.stderr, file=sys.stderr)
                write_results(args.out, results, all_symbols)
                raise SystemExit(f"{target}/{config}: build failed")
            results.setdefault(target, {})[config] = sizes
            all_symbols[(target, config)] = symbols
            print(
                f"{target:18s} {config:9s} flash={sizes['flash']:8d} "
                f"ram={sizes['ram']:8d} lib={sizes['library']['size']:8d}"
            )

    symbols_dir = write_results(args.out, results, all_symbols)
    print(f"\nwrote {args.out} and {len(all_symbols)} symbol maps in {symbols_dir}")


if __name__ == "__main__":
    main()
