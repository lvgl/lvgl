# LVGL Memory Usage Tests

Bare-metal build configurations used to measure LVGL's real flash and RAM footprint from a
fully linked executable.

Three configurations are measured on four targets:

- `standard/`: a representative configuration with a typical feature set enabled (widgets,
  fonts, every software renderer colour format), meant to reflect a "normal" app.
- `rgb565/`: an RGB565 panel with only the renderer colour formats a program like this needs.
- `minimal/`: as many features disabled as possible, as a baseline for LVGL's core cost.

| Target | Toolchain |
|---|---|
| `arm-cortex-m4` | `arm-none-eabi-gcc` |
| `arm-cortex-m0plus` | `arm-none-eabi-gcc` |
| `xtensa-esp32` | `xtensa-esp32-elf-gcc` |
| `riscv32-esp32c3` | `riscv32-esp-elf-gcc` |

## Prerequisites

The measurement script needs the toolchains on `PATH`, `cmake`, `ninja`, and `kconfiglib`.

```bash
# ARM
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
tar xf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

# Xtensa and RISC-V
ESP_VER=13.2.0_20230928
for tc in xtensa-esp-elf riscv32-esp-elf; do
    wget "https://github.com/espressif/crosstool-NG/releases/download/esp-$ESP_VER/$tc-$ESP_VER-x86_64-linux-gnu.tar.xz"
    tar xf "$tc-$ESP_VER-x86_64-linux-gnu.tar.xz"
done

export PATH=$PWD/gcc-arm-none-eabi-10.3-2021.10/bin:$PWD/xtensa-esp-elf/bin:$PWD/riscv32-esp-elf/bin:$PATH
```

## Measuring

```bash
./measure.py
./measure.py --targets arm-cortex-m4 --configs standard
./measure.py --skip-missing
```

It writes `size-results.json`, a per-symbol map per target and configuration under
`symbols/`, and prints a summary:

```
arm-cortex-m4      standard  flash=  370812 ram=  374328 lib=  362804
arm-cortex-m4      rgb565      flash=  332884 ram=  374328 lib=  328113
arm-cortex-m4      minimal   flash=  140672 ram=   77560 lib=  250430
...
```

- **Flash** = `text + data` (code, rodata, and the flash-resident copy of initialised globals)
- **RAM** = `data + bss` (writable globals plus zero-initialised globals)
- **lib** = `liblvgl.a` in full, with nothing dead-stripped

```bash
# largest symbols
arm-none-eabi-nm --size-sort -S --radix=d build/arm-cortex-m4-standard/main | tail -40

# per translation unit, which is usually the quickest way to spot a jump
find build/arm-cortex-m4-standard/lvgl/CMakeFiles/lvgl.dir/src -name '*.obj' \
    -exec arm-none-eabi-size {} +
```

## Adding a target

Add a toolchain file under `toolchain/` and an entry in `measure.py`'s `TARGETS`. Any target
specific requirements live in the toolchain file.
