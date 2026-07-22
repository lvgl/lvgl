cmake_minimum_required(VERSION 3.12.4)
project(lvgl_test_flash_usage)

get_filename_component(REPO_ROOT
                       ${CMAKE_CURRENT_LIST_DIR}/../../..
                       ABSOLUTE)

set(LV_BUILD_USE_KCONFIG ON 
    CACHE BOOL "use kconfig")

set(LV_BUILD_DEFCONFIG_PATH "${REPO_ROOT}/configs/defconfigs/minimal.defconfig" 
    CACHE PATH "defconfig path")

add_compile_options(-Os -ffunction-sections -fdata-sections -mcpu=cortex-m4 -mthumb)
add_subdirectory(${REPO_ROOT} lvgl)

add_executable(main main.c logo.c)
target_compile_definitions(main PRIVATE LV_BUILD_TEST)
target_link_libraries(main PUBLIC lvgl::lvgl)

target_link_options(main PRIVATE
    -T${CMAKE_SOURCE_DIR}/link.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/flash_test.map
    -Wl,--entry=Reset_Handler
)
