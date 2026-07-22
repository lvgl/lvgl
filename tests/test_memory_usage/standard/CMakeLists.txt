cmake_minimum_required(VERSION 3.12.4)
project(lvgl_test_flash_usage)

get_filename_component(REPO_ROOT
                       ${CMAKE_CURRENT_LIST_DIR}/../../..
                       ABSOLUTE)

set(LV_BUILD_USE_KCONFIG ON 
    CACHE BOOL "use kconfig")

set(LV_BUILD_DEFCONFIG_PATH "${CMAKE_CURRENT_LIST_DIR}/standard.defconfig" 
    CACHE PATH "defconfig path")

add_compile_options(-Os -ffunction-sections -fdata-sections -mcpu=cortex-m4 -mthumb)
add_subdirectory(${REPO_ROOT} lvgl)

add_executable(main main.c)
target_link_libraries(main PUBLIC lvgl::lvgl lvgl::demos)

target_link_options(main PRIVATE
    -T${CMAKE_SOURCE_DIR}/link.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/flash_test.map
    -Wl,--entry=Reset_Handler
)
