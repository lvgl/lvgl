cmake_minimum_required(VERSION 3.12.4)
project(lvgl_test_memory_usage_rgb565)

set(LV_MEM_TEST_DEFCONFIG "${CMAKE_CURRENT_LIST_DIR}/rgb565.defconfig")
set(LV_MEM_TEST_SOURCES ../common/app_widgets.c)
set(LV_MEM_TEST_LIBS lvgl::lvgl lvgl::demos)

include(${CMAKE_CURRENT_LIST_DIR}/../common/common.cmake)
