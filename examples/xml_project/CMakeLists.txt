# Generated file, do not edit.
# This file is used to compile the UI Project created by the Editor.
# See `user_config.cmake` as a way to add your own source files here

# Only set project if this is the top-level CMakeLists.txt
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  cmake_minimum_required(VERSION 3.10)
  # can be customized
  project(LVGLProject)
  set(IS_TOP_LEVEL TRUE)
else()
  set(IS_TOP_LEVEL FALSE)
endif()

# Handles giving component libraries unique names when included as subprojects.
if(DEFINED LVGL_COMPONENT_LIB_NAME)
  set(LIB_NAME ${LVGL_COMPONENT_LIB_NAME})
else()
  set(LIB_NAME lib-ui)
endif()

# This includes the generated list of .c files
include(${CMAKE_CURRENT_LIST_DIR}/file_list_gen.cmake)
# Include any component libraries
include(${CMAKE_CURRENT_LIST_DIR}/component_lib_list_gen.cmake)
# Include extra source files that the user added
include(${CMAKE_CURRENT_LIST_DIR}/user_config.cmake)

# Create the UI sources as a library
add_library(${LIB_NAME} ${LV_EDITOR_PROJECT_SOURCES})

if(DEFINED LV_EDITOR_COMPONENT_LIB_LIST)
  target_link_libraries(${LIB_NAME} PUBLIC ${LV_EDITOR_COMPONENT_LIB_LIST})
endif()

if(TARGET lvgl)
  target_link_libraries(${LIB_NAME} PUBLIC lvgl)
endif()

# Add the project folder as include folder
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set include directories for the project This will include the LVGL includes
target_include_directories(${LIB_NAME} PRIVATE ${COMMON_INCLUDE_DIRS})
