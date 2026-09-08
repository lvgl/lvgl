# Shared build rules for the memory-usage tests.
#
# Each configuration's CMakeLists.txt sets the variables below and then includes this file:
#   LV_MEM_TEST_DEFCONFIG - defconfig describing the LVGL configuration to measure
#   LV_MEM_TEST_SOURCES   - sources of the test application
#   LV_MEM_TEST_LIBS      - LVGL targets to link against
#   LV_MEM_TEST_DEFINITIONS - optional extra compile definitions for the test application
#
# Everything target specific (the architecture flags, whether the ROM stubs are needed and
# which extra libraries have to be linked) comes from the toolchain file, so a configuration
# never has to know which target it is being built for.

get_filename_component(LV_MEM_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
get_filename_component(REPO_ROOT ${LV_MEM_TEST_DIR}/../.. ABSOLUTE)

set(LV_BUILD_USE_KCONFIG ON
    CACHE BOOL "use kconfig")

set(LV_BUILD_DEFCONFIG_PATH "${LV_MEM_TEST_DEFCONFIG}"
    CACHE PATH "defconfig path")

add_compile_options(-Os -ffunction-sections -fdata-sections)
add_subdirectory(${REPO_ROOT} lvgl)

add_executable(main ${LV_MEM_TEST_SOURCES})

if(LV_MEM_TEST_DEFINITIONS)
    target_compile_definitions(main PRIVATE ${LV_MEM_TEST_DEFINITIONS})
endif()

if(LV_MEM_TEST_NEEDS_ROM_STUBS)
    target_sources(main PRIVATE ${LV_MEM_TEST_DIR}/common/rom_stubs.c)
endif()

target_link_libraries(main PUBLIC ${LV_MEM_TEST_LIBS} ${LV_MEM_TEST_EXTRA_LIBS})

target_link_options(main PRIVATE
    -T${LV_MEM_TEST_DIR}/common/link.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/flash_test.map
    -Wl,--entry=Reset_Handler
)
