# Toolchains and emulators for the QEMU instruction-count benchmark.
#
#   docker build -t lv-perf-qemu tests/perf_qemu
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

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

# Fail the build if a required tool is missing
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
