include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

# Option to define LV_LVGL_H_INCLUDE_SIMPLE, default: ON
option(LV_LVGL_H_INCLUDE_SIMPLE
       "Use #include \"lvgl.h\" instead of #include \"../../lvgl.h\"" ON)

# Option to define LV_CONF_INCLUDE_SIMPLE, default: ON
option(LV_CONF_INCLUDE_SIMPLE
       "Use #include \"lv_conf.h\" instead of #include \"../../lv_conf.h\"" ON)

set(LV_KCONFIG_IGNORE "Don't use Kconfig. Kconfig is not used by default." ON)

find_package(Python REQUIRED)

option(LV_CONF_SKIP "Skip including lv_conf.h during configuration" OFF)
option(LV_USE_KCONFIG "Use Kconfig to configure LVGL" OFF)

# Option LV_CONF_PATH, which should be the path for lv_conf.h
# If set parent path LV_CONF_DIR is added to includes
if( LV_CONF_PATH )
    get_filename_component(LV_CONF_DIR ${LV_CONF_PATH} DIRECTORY)
endif( LV_CONF_PATH )

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

set(PCPP_DEFINITIONS_LIST "")

if ( LV_USE_KCONFIG )
  # kconfig.cmake will generate the .config
  # and autoconf.h, which will be used by lv_conf_kconfig.h
  include(${CMAKE_CURRENT_LIST_DIR}/kconfig.cmake)

  set(LV_KCONFIG_IGNORE OFF)
  # Set the flag to specify we are using kconfig, needed for the
  # generate_cmake_variables.py script
  set(KCONFIG_FLAG --kconfig)
  # If using Kconfig, we need to define additional definitions
  list(APPEND PCPP_DEFINITIONS_LIST "LV_CONF_SKIP" "LV_CONF_KCONFIG_EXTERNAL_INCLUDE=\"${LV_CONF_KCONFIG_EXTERNAL_INCLUDE}\"")
else()
  if (LV_CONF_PATH)
    list(APPEND PCPP_DEFINITIONS_LIST "LV_CONF_PATH=\"${LV_CONF_PATH}\"")
  endif()

  if (LV_LVGL_H_INCLUDE_SIMPLE)
    list(APPEND PCPP_DEFINITIONS_LIST "LV_LVGL_H_INCLUDE_SIMPLE")
  endif()

  if (LV_CONF_INCLUDE_SIMPLE)
    list(APPEND PCPP_DEFINITIONS_LIST "LV_CONF_INCLUDE_SIMPLE")
  endif()
endif()

if(PCPP_DEFINITIONS_LIST)
  set(PCPP_ADDITIONAL_DEFS "--defs" ${PCPP_DEFINITIONS_LIST})
else()
  set(PCPP_ADDITIONAL_DEFS "")
endif()


target_compile_definitions(
  lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_KCONFIG_IGNORE}>:LV_KCONFIG_IGNORE>
              $<$<BOOL:${LV_CONF_SKIP}>:LV_CONF_SKIP>
)

# Use the portable pcpp to preprocess lv_conf_internal.h
execute_process(
  COMMAND ${Python_EXECUTABLE} ${LVGL_ROOT_DIR}/scripts/preprocess_lv_conf_internal.py
  --input ${LVGL_ROOT_DIR}/src/lv_conf_internal.h
  --tmp_file ${CMAKE_CURRENT_BINARY_DIR}/tmp.h
  --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
  ${PCPP_ADDITIONAL_DEFS}
  --include ${LVGL_ROOT_DIR} ${LVGL_ROOT_DIR}/.. ${LVGL_ROOT_DIR}/src ${LV_CONF_DIR}
  RESULT_VARIABLE ret
)
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "preprocess_lv_conf_internal.py failed with return code: ${ret}")
endif()

# Convert the expanded lv_conf_expanded.h to cmake variables
execute_process(
  COMMAND ${Python_EXECUTABLE}
    ${LVGL_ROOT_DIR}/scripts/generate_cmake_variables.py
    --input ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
    --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake
    ${KCONFIG_FLAG}
    RESULT_VARIABLE ret
)
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "generate_cmake_variables.py command failed with return code: ${ret}")
endif()

# This will set all CONFIG_LV_USE_* variables in cmake
include(${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake)

# Add definition of LV_CONF_PATH only if needed
# Do not redefine it if already defined in tests/CMakeLists.txt
if(LV_CONF_PATH AND NOT LV_BUILD_TEST)
  target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${LV_CONF_PATH})
endif()

# Add definition of LV_CONF_SKIP only if needed
if(LV_CONF_SKIP)
  target_compile_definitions(lvgl PUBLIC LV_CONF_SKIP=1)
endif()

# Include root and optional parent path of LV_CONF_PATH
target_include_directories(lvgl SYSTEM PUBLIC ${LVGL_ROOT_DIR} ${LV_CONF_DIR} ${CMAKE_CURRENT_BINARY_DIR})


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

if(NOT LV_CONF_SKIP)
  if (LV_CONF_PATH)
    string(REPLACE "\"" "" UNQUOTED_LV_CONF_PATH ${LV_CONF_PATH})
    list(APPEND LVGL_PUBLIC_HEADERS ${UNQUOTED_LV_CONF_PATH})
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


#Install headers
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
  DESTINATION "${INC_INSTALL_DIR}"
  FILES_MATCHING
  PATTERN "*.h")

# Install headers from the LVGL_PUBLIC_HEADERS variable
install(
  FILES ${LVGL_PUBLIC_HEADERS}
  DESTINATION "${INC_INSTALL_DIR}/"
)

# install example headers
if(NOT LV_CONF_BUILD_DISABLE_EXAMPLES)
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
    DESTINATION "${INC_INSTALL_DIR}"
    FILES_MATCHING
    PATTERN "*.h")
endif()

# install demo headers
if(NOT LV_CONF_BUILD_DISABLE_DEMOS)
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
    DESTINATION "${INC_INSTALL_DIR}"
    FILES_MATCHING
    PATTERN "*.h")
endif()


configure_file("${LVGL_ROOT_DIR}/lvgl.pc.in" ${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc @ONLY)
configure_file("${LVGL_ROOT_DIR}/lv_version.h.in" ${CMAKE_CURRENT_BINARY_DIR}/lv_version.h @ONLY)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
  DESTINATION "share/pkgconfig/")

# Install library
set_target_properties(
  lvgl
  PROPERTIES OUTPUT_NAME lvgl
             VERSION ${LVGL_VERSION}
             SOVERSION ${LVGL_SOVERSION}
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
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
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
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
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
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
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_examples
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")
endif()
