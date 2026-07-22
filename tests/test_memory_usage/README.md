# LVGL Memory Usage Tests

This directory contains bare-metal (Cortex-M4) build configurations used to
measure LVGL's real flash and RAM footprint via a fully linked executable,
built with `arm-none-eabi-gcc` and `--gc-sections`.

Two configurations are provided:

- `standard/` — a representative `lv_conf.h` with a typical/common feature set
  enabled (widgets, fonts, etc.), meant to reflect a "normal" app.
- `minimal/` — a stripped-down `lv_conf.h` with as many features disabled as
  possible, used as a baseline to measure LVGL's core cost.

Both configurations share the same toolchain file, and  differ in 
their linker scripts, lvgl configuration and the UI they run.

## Prerequisites

Download and extract the toolchain (only needs to be done once):

```bash
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
tar xvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
```

Add the toolchain's `bin/` directory to your `PATH` before running CMake:

```bash
export PATH=/path/to/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH
```

## Building

From either `standard/` or `minimal/`:

```bash
cmake -B build -GNinja -DCMAKE_TOOLCHAIN_FILE=../toolchain/arm-none-eabi.cmake
cmake --build build
```

## Measuring flash/RAM usage

```bash
arm-none-eabi-size build/main
```

Example output:

```
   text	  data	   bss	   dec	   hex	filename
 283472	   832	307936	592240	 90970	build/main
```

- **Flash usage** = `text + data` (code, rodata, and initialized globals
  stored in flash)
- **RAM usage** = `data + bss` (writable globals + zero-initialized globals)

For a per-symbol breakdown of what's contributing to the size (useful when
comparing `standard/` vs `minimal/`, or after enabling a new feature), use
the generated map file or `nm`:

```bash
arm-none-eabi-nm --size-sort -S build/main | tail -40
```
