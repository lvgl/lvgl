cmake_minimum_required(VERSION 3.12.4)
project(lvgl_test_memory_usage_minimal)

get_filename_component(LV_MEM_TEST_REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE)

set(LV_MEM_TEST_DEFCONFIG "${LV_MEM_TEST_REPO_ROOT}/configs/defconfigs/minimal.defconfig")
set(LV_MEM_TEST_SOURCES main.c logo.c)
set(LV_MEM_TEST_LIBS lvgl::lvgl)
set(LV_MEM_TEST_DEFINITIONS LV_BUILD_TEST)

include(${CMAKE_CURRENT_LIST_DIR}/../common/common.cmake)
