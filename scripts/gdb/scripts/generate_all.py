#!/usr/bin/env python3
"""
Unified entry point for all LVGL GDB constant generators.

Scans the generators/ subdirectory for gen_*.py modules and calls
each module's main() function.

Usage (from the GDB script root):
    python3 scripts/generate_all.py
"""

import importlib.util
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GENERATORS_DIR = SCRIPT_DIR / "generators"

# Make enum_parser importable from generators
sys.path.insert(0, str(SCRIPT_DIR))


def main():
    gen_files = sorted(GENERATORS_DIR.glob("gen_*.py"))
    if not gen_files:
        print("No generators found.")
        return

    print(f"Running {len(gen_files)} generator(s)...\n")

    for gen_file in gen_files:
        module_name = gen_file.stem
        print(f"  [{module_name}]")
        spec = importlib.util.spec_from_file_location(module_name, gen_file)
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        mod.main()
        print()

    print("All generators completed.")


if __name__ == "__main__":
    main()
