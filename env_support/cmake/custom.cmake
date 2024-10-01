include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

# Option to define LV_LVGL_H_INCLUDE_SIMPLE, default: ON
option(LV_LVGL_H_INCLUDE_SIMPLE
       "Use #include \"lvgl.h\" instead of #include \"../../lvgl.h\"" ON)

# Option to define LV_CONF_INCLUDE_SIMPLE, default: ON
option(LV_CONF_INCLUDE_SIMPLE
       "Use #include \"lv_conf.h\" instead of #include \"../../lv_conf.h\"" ON)

set(LV_CONF_PATH "" CACHE STRING
    "Path to lv_conf.h.")
set(LV_CONF_DEFCONFIG_PATH "" CACHE STRING
    "Generate lv_conf.h from the given defconfig. If set, LV_CONF_PATH is unset and LV_CONF_SKIP is set to ON.")

if(LV_CONF_DEFCONFIG_PATH)
  set(LV_CONF_SKIP ON)
  unset(LV_CONF_PATH)
  set(KCONFIG_LV_CONF_DIR "${CMAKE_BINARY_DIR}/lv_kconfig_generated")
  set(KCONFIG_LV_CONF_PATH "${KCONFIG_LV_CONF_DIR}/lv_kconfig_generated.h")
  make_directory("${KCONFIG_LV_CONF_DIR}")
  add_custom_command(
    OUTPUT "${KCONFIG_LV_CONF_PATH}"
    COMMAND
    env KCONFIG_CONFIG='${LV_CONF_DEFCONFIG_PATH}'
    '${LVGL_ROOT_DIR}/scripts/genconfig.py'
    --header-path='${KCONFIG_LV_CONF_PATH}'
    '${LVGL_ROOT_DIR}/Kconfig'
    DEPENDS
    "${LV_CONF_DEFCONFIG_PATH}"
    "${LVGL_ROOT_DIR}/Kconfig"
  )
  add_custom_target(lv_kconfig_generate_command DEPENDS "${KCONFIG_LV_CONF_PATH}")
  add_library(lv_kconfig_generated INTERFACE)
  add_dependencies(lv_kconfig_generated lv_kconfig_generate_command)
  target_compile_definitions(lv_kconfig_generated INTERFACE LV_CONF_KCONFIG_EXTERNAL_INCLUDE=<lv_kconfig_generated.h>)
  target_include_directories(lv_kconfig_generated SYSTEM INTERFACE "${KCONFIG_LV_CONF_DIR}")
endif()

# Option to build shared libraries (as opposed to static), default: OFF
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Set sources used for LVGL components
file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c ${LVGL_ROOT_DIR}/src/*.S)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)
file(GLOB_RECURSE THORVG_SOURCES ${LVGL_ROOT_DIR}/src/libs/thorvg/*.cpp ${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/*.cpp)

# Build LVGL library
add_library(lvgl ${SOURCES})
add_library(lvgl::lvgl ALIAS lvgl)

target_compile_definitions(
  lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>)

if(LV_CONF_DEFCONFIG_PATH)
  target_link_libraries(lvgl PUBLIC lv_kconfig_generated)
endif()

# Add definition and include directory of LV_CONF_PATH only if needed
if(LV_CONF_PATH)
  target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${LV_CONF_PATH})
  get_filename_component(LV_CONF_DIR "${LV_CONF_PATH}" DIRECTORY)
  target_include_directories(lvgl SYSTEM PUBLIC ${LV_CONF_DIR})
endif()

# Add definition of LV_CONF_SKIP only if needed
if(LV_CONF_SKIP)
  target_compile_definitions(lvgl PUBLIC LV_CONF_SKIP=1)
endif()

target_include_directories(lvgl SYSTEM PUBLIC ${LVGL_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(NOT LV_CONF_BUILD_DISABLE_THORVG_INTERNAL)
    add_library(lvgl_thorvg ${THORVG_SOURCES})
    add_library(lvgl::thorvg ALIAS lvgl_thorvg)
    target_include_directories(lvgl_thorvg SYSTEM PUBLIC ${LVGL_ROOT_DIR}/src/libs/thorvg)
    target_link_libraries(lvgl_thorvg PUBLIC lvgl)
endif()

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
  set_source_files_properties(${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/vg_lite_tvg.cpp PROPERTIES COMPILE_FLAGS -Wunused-parameter)
endif()

# Build LVGL example library
if(NOT LV_CONF_BUILD_DISABLE_EXAMPLES)
    add_library(lvgl_examples ${EXAMPLE_SOURCES})
    add_library(lvgl::examples ALIAS lvgl_examples)

    target_include_directories(lvgl_examples SYSTEM PUBLIC ${LVGL_ROOT_DIR}/examples)
    target_link_libraries(lvgl_examples PUBLIC lvgl)
endif()

# Build LVGL demos library
if(NOT LV_CONF_BUILD_DISABLE_DEMOS)
    add_library(lvgl_demos ${DEMO_SOURCES})
    add_library(lvgl::demos ALIAS lvgl_demos)

    target_include_directories(lvgl_demos SYSTEM PUBLIC ${LVGL_ROOT_DIR}/demos)
    target_link_libraries(lvgl_demos PUBLIC lvgl)
endif()

# Library and headers can be installed to system using make install
file(GLOB LVGL_PUBLIC_HEADERS
    "${LVGL_ROOT_DIR}/lvgl.h"
    "${LVGL_ROOT_DIR}/lv_version.h")

if(LV_CONF_DEFCONFIG_PATH)
  list(APPEND LVGL_PUBLIC_HEADERS "${KCONFIG_LV_CONF_PATH}")
endif()

if(NOT LV_CONF_SKIP)
  if (LV_CONF_PATH)
    list(APPEND LVGL_PUBLIC_HEADERS
    ${LV_CONF_PATH})
  else()
    list(APPEND LVGL_PUBLIC_HEADERS
    "${CMAKE_SOURCE_DIR}/lv_conf.h")
  endif()
endif()

if("${LIB_INSTALL_DIR}" STREQUAL "")
  set(LIB_INSTALL_DIR "lib")
endif()
if("${RUNTIME_INSTALL_DIR}" STREQUAL "")
  set(RUNTIME_INSTALL_DIR "bin")
endif()
if("${INC_INSTALL_DIR}" STREQUAL "")
  set(INC_INSTALL_DIR "include/lvgl")
endif()

set(CMAKE_INSTALL_PREFIX .)

#Install headers
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
  FILES_MATCHING
  PATTERN "*.h")

# Install headers from the LVGL_PUBLIC_HEADERS variable
install(
  FILES ${LVGL_PUBLIC_HEADERS}
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
)

# install example headers
if(NOT LV_CONF_BUILD_DISABLE_EXAMPLES)
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
    FILES_MATCHING
    PATTERN "*.h")
endif()

# install demo headers
if(NOT LV_CONF_BUILD_DISABLE_DEMOS)
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
    FILES_MATCHING
    PATTERN "*.h")
endif()


configure_file("${LVGL_ROOT_DIR}/lvgl.pc.in" ${CMAKE_BINARY_DIR}/lvgl.pc @ONLY)
configure_file("${LVGL_ROOT_DIR}/lv_version.h.in" ${CMAKE_BINARY_DIR}/lv_version.h @ONLY)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
  DESTINATION "${LIB_INSTALL_DIR}/pkgconfig/")

# Install library
set_target_properties(
  lvgl
  PROPERTIES OUTPUT_NAME lvgl
             VERSION ${LVGL_VERSION}
             SOVERSION ${LVGL_SOVERSION}
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
             PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

install(
  TARGETS lvgl
  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
  RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
  PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")


# Install library thorvg
if(NOT LV_CONF_BUILD_DISABLE_THORVG_INTERNAL)
  set_target_properties(
    lvgl_thorvg
    PROPERTIES OUTPUT_NAME lvgl_thorvg
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_thorvg
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
endif()

# Install library demos
if(NOT LV_CONF_BUILD_DISABLE_DEMOS)
  set_target_properties(
    lvgl_demos
    PROPERTIES OUTPUT_NAME lvgl_demos
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_demos
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
endif()

#install library examples
if(NOT LV_CONF_BUILD_DISABLE_EXAMPLES)
  set_target_properties(
    lvgl_examples
    PROPERTIES OUTPUT_NAME lvgl_examples
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_examples
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
endif()
