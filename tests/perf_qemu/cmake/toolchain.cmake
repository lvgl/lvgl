# Cross toolchain for the QEMU instruction-count benchmark. One branch per target,
# selected with -DPERF_QEMU_TARGET. The matching QEMU invocation is in perf_qemu.py
# and the matching linker script and startup file are in harness/targets/<target>.
#
# The two Cortex-M targets use newlib and the other two picolibc, because that is what
# Ubuntu ships for each of them.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# There is no startup file or linker script until the harness adds one, so a
# try-compile that links a whole executable would fail before it proves anything.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# CMake re-reads this file inside its try-compile projects, where a -D on the outer
# command line is not visible, so the target selection has to be carried over.
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES PERF_QEMU_TARGET)

if(PERF_QEMU_TARGET STREQUAL "cortex-m7")
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(_prefix arm-none-eabi-)
    # The classic software-rendering MCU, and the target the numbers in the session notes
    # were taken on. -mfloat-abi=hard because LV_USE_FLOAT is on and the M7 has an FPU.
    set(_arch "-mcpu=cortex-m7 -mthumb -mfloat-abi=hard")
    set(_libc "--specs=nosys.specs")
elseif(PERF_QEMU_TARGET STREQUAL "cortex-m55")
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(_prefix arm-none-eabi-)
    # +nomve for two reasons. It keeps this a software-rendering measurement: with MVE
    # available GCC's -O2 auto-vectorizes into it, which is not the scalar C path the
    # other two targets run. And it dodges a build failure: lv_blend_helium.S opens on
    # __ARM_FEATURE_MVE and includes lvgl_public.h before it checks
    # LV_USE_NATIVE_HELIUM_ASM, and those headers do not honour __ASSEMBLY__, so the
    # assembler sees C declarations and fails whatever the config says.
    # The FPU stays: LV_USE_FLOAT is on, and -mfloat-abi=hard is what an M55 build uses.
    set(_arch "-mcpu=cortex-m55+nomve -mthumb -mfloat-abi=hard")
    set(_libc "--specs=nosys.specs")
elseif(PERF_QEMU_TARGET STREQUAL "riscv32")
    set(CMAKE_SYSTEM_PROCESSOR riscv)
    set(_prefix riscv64-unknown-elf-)
    # rv32imac is the common MCU core: integer only, so LV_USE_FLOAT goes through
    # libgcc's soft float. Deterministic, just slower than a target with an FPU.
    set(_arch "-march=rv32imac_zicsr -mabi=ilp32")
    set(_libc "--specs=picolibc.specs")
elseif(PERF_QEMU_TARGET STREQUAL "cortex-a53")
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(_prefix aarch64-linux-gnu-)
    set(_arch "-mcpu=cortex-a53")
    set(_libc "--specs=picolibc.specs")
else()
    message(FATAL_ERROR
        "set -DPERF_QEMU_TARGET to cortex-m7, cortex-m55, riscv32 or cortex-a53, "
        "not '${PERF_QEMU_TARGET}'")
endif()

set(CMAKE_C_COMPILER   ${_prefix}gcc)
set(CMAKE_CXX_COMPILER ${_prefix}g++)
set(CMAKE_ASM_COMPILER ${_prefix}gcc)
set(CMAKE_AR           ${_prefix}ar)
set(CMAKE_OBJCOPY      ${_prefix}objcopy)
set(CMAKE_OBJDUMP      ${_prefix}objdump)
set(CMAKE_NM           ${_prefix}nm)
set(CMAKE_SIZE         ${_prefix}size)

# The specs file has to be on the compile line as well as the link line: picolibc's is
# what puts its own headers on the include path, and without it the very first source
# fails on a missing inttypes.h.
set(CMAKE_C_FLAGS_INIT "${_arch} ${_libc} -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS_INIT "${_arch} ${_libc}")
# LVGL's source list has one C++ file, src/debugging/vg_lite_tvg/vg_lite_tvg.cpp. Its body
# is behind LV_USE_VG_LITE_THORVG and compiles to nothing here, but it still has to find
# the libc headers, so C++ needs the same flags as C.
set(CMAKE_CXX_FLAGS_INIT "${_arch} ${_libc} -ffunction-sections -fdata-sections")

# The optimisation level is the benchmark's own, not CMake's, because Release would
# otherwise be -O3 and the level is one of the things being compared. CI uses -O2.
if(NOT PERF_QEMU_OPT)
    set(PERF_QEMU_OPT O2)
endif()
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES PERF_QEMU_OPT)
set(CMAKE_C_FLAGS_RELEASE "-${PERF_QEMU_OPT} -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-${PERF_QEMU_OPT} -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_RELEASE "-${PERF_QEMU_OPT}" CACHE STRING "" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
