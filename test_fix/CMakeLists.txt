cmake_minimum_required(VERSION 3.10.0)
project(TestFix)

include(FetchContent)
FetchContent_Declare(lvgl 
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lvgl
)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(lvgl)

add_executable(TestFix main.c)
target_link_libraries(TestFix PRIVATE lvgl)
