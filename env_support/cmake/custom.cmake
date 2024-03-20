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

# Option to build shared libraries (as opposed to static), default: OFF
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)

if (BUILD_SHARED_LIBS)
  add_library(lvgl SHARED ${SOURCES})
else()
  add_library(lvgl STATIC ${SOURCES})
endif()

add_library(lvgl::lvgl ALIAS lvgl)
add_library(lvgl_examples STATIC ${EXAMPLE_SOURCES})
add_library(lvgl::examples ALIAS lvgl_examples)
add_library(lvgl_demos STATIC ${DEMO_SOURCES})
add_library(lvgl::demos ALIAS lvgl_demos)

target_compile_definitions(
  lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>)

# Include root and optional parent path of LV_CONF_PATH
target_include_directories(lvgl SYSTEM PUBLIC ${LVGL_ROOT_DIR} ${LV_CONF_DIR})

# Include /examples folder
target_include_directories(lvgl_examples SYSTEM
                           PUBLIC ${LVGL_ROOT_DIR}/examples)
target_include_directories(lvgl_demos SYSTEM
                           PUBLIC ${LVGL_ROOT_DIR}/demos)

target_link_libraries(lvgl_examples PUBLIC lvgl)
target_link_libraries(lvgl_demos PUBLIC lvgl)

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

install(
  FILES "${LV_CONF_PATH}"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/../"
  RENAME "lv_conf.h"
  OPTIONAL)

configure_file("${LVGL_ROOT_DIR}/lvgl.pc.in" lvgl.pc @ONLY)

install(
  FILES "${CMAKE_BINARY_DIR}/lvgl.pc"
  DESTINATION "${LIB_INSTALL_DIR}/pkgconfig/")

set_target_properties(
  lvgl
  PROPERTIES OUTPUT_NAME lvgl
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

install(
  TARGETS lvgl
  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
  RUNTIME DESTINATION "${LIB_INSTALL_DIR}"
  PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
