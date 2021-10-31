# Option to define LV_LVGL_H_INCLUDE_SIMPLE, default: ON
option(LV_LVGL_H_INCLUDE_SIMPLE
       "Use #include \"lvgl.h\" instead of #include \"../../lvgl.h\"" ON)

# Option to define LV_CONF_INCLUDE_SIMPLE, default: ON
option(LV_CONF_INCLUDE_SIMPLE
       "Simple include of \"lv_conf.h\" and \"lv_drv_conf.h\"" ON)

# Option to set LV_CONF_PATH, if set parent path LV_CONF_DIR is added to
# includes
option(LV_CONF_PATH "Path defined for lv_conf.h")
get_filename_component(LV_CONF_DIR ${LV_CONF_PATH} DIRECTORY)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE EXAMPLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.c)

add_library(lvgl STATIC ${SOURCES})
add_library(lvgl::lvgl ALIAS lvgl)
add_library(lvgl_examples STATIC ${EXAMPLE_SOURCES})
add_library(lvgl::examples ALIAS lvgl_examples)

target_compile_definitions(
  lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>)

# Include root and optional parent path of LV_CONF_PATH
target_include_directories(lvgl SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                              ${LV_CONF_DIR})

# Include /examples folder
target_include_directories(lvgl_examples SYSTEM
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/examples)

target_link_libraries(lvgl_examples PUBLIC lvgl)

# Lbrary and headers can be installed to system using make install
file(GLOB LVGL_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/lv_conf.h"
     "${CMAKE_SOURCE_DIR}/lvgl.h")

if("${LIB_INSTALL_DIR}" STREQUAL "")
  set(LIB_INSTALL_DIR "lib")
endif()
if("${INC_INSTALL_DIR}" STREQUAL "")
  set(INC_INSTALL_DIR "include/lvgl")
endif()

install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/src"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
  FILES_MATCHING
  PATTERN "*.h")

set_target_properties(
  lvgl
  PROPERTIES OUTPUT_NAME lvgl
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

install(
  TARGETS lvgl
  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
  PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
