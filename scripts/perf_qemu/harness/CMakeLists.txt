# Bare metal benchmark images for one target.
#
# The tree under measurement is passed in as PERF_QEMU_LVGL_DIR rather than being
# this directory's parent, because the same harness has to build a baseline worktree,
# which does not contain scripts/perf_qemu at all.
cmake_minimum_required(VERSION 3.16)

if(NOT PERF_QEMU_LVGL_DIR)
    message(FATAL_ERROR "set -DPERF_QEMU_LVGL_DIR to the LVGL tree to measure")
endif()
get_filename_component(PERF_QEMU_LVGL_DIR "${PERF_QEMU_LVGL_DIR}" ABSOLUTE)

project(perf_qemu C ASM)

set(_here ${CMAKE_CURRENT_LIST_DIR})
set(_target_dir ${_here}/targets/${PERF_QEMU_TARGET})
if(NOT EXISTS ${_target_dir}/link.ld)
    message(FATAL_ERROR "no linker script in ${_target_dir}")
endif()

# The two Cortex-M boards differ only in their memory map and their SysTick rate, so they
# share one startup file. The other targets have their own.
if(PERF_QEMU_TARGET MATCHES "^cortex-m")
    set(_startup ${_here}/startup_cortex_m.c)
else()
    set(_startup ${_target_dir}/startup.c)
endif()

# Which colour formats to build an image for. perf_qemu.py passes the ones it will run,
# so a single-format run does not link the other eight.
if(NOT PERF_QEMU_FORMATS)
    message(FATAL_ERROR "set -DPERF_QEMU_FORMATS to a ;-separated list of colour formats")
endif()

# The config comes from the harness but is resolved against the measured tree's own
# Kconfig, so both sides of a comparison are configured identically.
set(LV_BUILD_USE_KCONFIG ON CACHE BOOL "" FORCE)
set(LV_BUILD_DEFCONFIG_STRICT ON CACHE BOOL "" FORCE)
set(LV_BUILD_DEFCONFIG_PATH "${_here}/../configs/perf_qemu.defconfig" CACHE STRING "" FORCE)

# Bare metal: there is nothing to find, and fetching a dependency would make the
# build depend on the network.
set(LV_USE_FIND_PACKAGE OFF CACHE BOOL "" FORCE)
set(LV_USE_PKG_CONFIG OFF CACHE BOOL "" FORCE)
set(LV_FETCH_DEPENDENCIES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS ON CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG OFF CACHE BOOL "" FORCE)

add_subdirectory(${PERF_QEMU_LVGL_DIR} lvgl EXCLUDE_FROM_ALL)

# One image per (suite, colour format). The colour format is a compile time define
# so that the LVGL archive is built once and only the small main is relinked.
foreach(suite bench render)
    foreach(cf IN LISTS PERF_QEMU_FORMATS)
        set(_exe ${suite}_${cf})
        add_executable(${_exe} ${_here}/main_${suite}.c ${_startup})
        target_include_directories(${_exe} PRIVATE ${_here} ${_target_dir})
        target_compile_definitions(${_exe} PRIVATE PERF_QEMU_CF=PERF_QEMU_CF_${cf})
        target_link_libraries(${_exe} PRIVATE lvgl lvgl::demos)
        # The libc specs file is already in CMAKE_C_FLAGS, which CMake also puts on the
        # link line. Repeating it here makes newlib's nosys.specs fail with "attempt to
        # rename spec 'link_gcc_c_sequence'".
        target_link_options(${_exe} PRIVATE
            -nostartfiles
            -T${_target_dir}/link.ld
            -Wl,--gc-sections
            -Wl,-Map=${_exe}.map
        )
        set_target_properties(${_exe} PROPERTIES SUFFIX ".elf"
            LINK_DEPENDS ${_target_dir}/link.ld)
    endforeach()
endforeach()
