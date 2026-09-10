# Toolchains and emulators for the QEMU instruction-count benchmark.
#
# The point of pinning this by digest in CI is that the numbers only mean
# something when the compiler and the emulator are identical everywhere. Run the
# same image locally through scripts/perf_qemu/run.sh and you get the same counts.
#
#   docker build -t lv-perf-qemu scripts/perf_qemu
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# binutils-riscv64-unknown-elf is not pulled in by its gcc package, and without it
# there is no `ar` and `as` rejects -march=rv32imac_zicsr.
#
# ca-certificates is load bearing and easy to miss, because --no-install-recommends leaves
# it out: actions/checkout runs git inside this image, and with no CA bundle every fetch
# fails with "server certificate verification failed" long before anything is measured.
RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libnewlib-dev \
        gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf picolibc-riscv64-unknown-elf \
        gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu \
        picolibc-aarch64-linux-gnu \
        qemu-system-arm qemu-system-misc \
        gdb-multiarch \
        cmake ninja-build make python3 python3-pil python3-numpy ccache git file \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Fail the build rather than the benchmark if a tool or the Cortex-M55 machine is missing.
# LVGL's CMakeLists enables C and CXX unconditionally, so every g++ has to be there too.
RUN arm-none-eabi-gcc --version >/dev/null \
    && riscv64-unknown-elf-gcc --version >/dev/null \
    && aarch64-linux-gnu-gcc --version >/dev/null \
    && arm-none-eabi-g++ --version >/dev/null \
    && riscv64-unknown-elf-g++ --version >/dev/null \
    && aarch64-linux-gnu-g++ --version >/dev/null \
    && qemu-system-arm -M help | grep -q mps3-an547 \
    && qemu-system-aarch64 --version >/dev/null \
    && qemu-system-riscv32 --version >/dev/null \
    && gdb-multiarch --version >/dev/null \
    && test -f /etc/ssl/certs/ca-certificates.crt

WORKDIR /lvgl
