cmake_minimum_required(VERSION 3.12)
project (lv_emscripten)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s USE_SDL=2")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(lvgl)
file(GLOB MY_SOURCES "./*.c")
set(SOURCES ${MY_SOURCES})

add_executable(index ${SOURCES} ${INCLUDES})

if(NOT LVGL_CHOSEN_DEMO)
    set(LVGL_CHOSEN_DEMO lv_demo_widgets)
endif()
set_source_files_properties(main.c PROPERTIES COMPILE_FLAGS -DCHOSEN_DEMO=${LVGL_CHOSEN_DEMO})

set(CMAKE_EXECUTABLE_SUFFIX ".html")
target_link_libraries(index
    lvgl
    lvgl_examples
    lvgl_demos
)
set_target_properties(index PROPERTIES LINK_FLAGS "--shell-file ${PROJECT_SOURCE_DIR}/lvgl/.devcontainer/lvgl_shell.html -s SINGLE_FILE=1")