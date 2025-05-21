cmake_minimum_required(VERSION 3.12.4)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT ESP_PLATFORM)
  project(lvgl LANGUAGES C HOMEPAGE_URL https://github.com/lvgl/lvgl)
  if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    enable_language(CXX ASM)
  else()
    enable_language(CXX)
  endif()
endif()

set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

if(ESP_PLATFORM)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/esp.cmake)
elseif(ZEPHYR_BASE)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/zephyr.cmake)
elseif(MICROPY_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/micropython.cmake)
else()
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/custom.cmake)
endif()

#[[
    unfortunately CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS does not work for global data. 
    for global data we still need decl specs.
    Check out the docs to learn more about the limitations of CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html#prop_tgt:WINDOWS_EXPORT_ALL_SYMBOLS

    For all compiled sources within the library (i.e. basically all lvgl files) we need to use dllexport.
    For all compiled sources from outside the library (i.e. files which include lvgl headers) we need to use dllimport.
    We can do this by using CMakes INTERFACE and PRIVATE keyword.
  ]]
if (MSVC)
  target_compile_definitions(lvgl
    INTERFACE LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllimport\)
    PRIVATE LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllexport\)
  )
endif()

if(LV_GENERATE_XDG)
  # Locate wayland-scanner tool
  find_program(WAYLAND_SCANNER_EXECUTABLE wayland-scanner REQUIRED)

  # Set the path to the xdg-shell.xml (allow override via cache)
  set(XDG_SHELL_XML_PATH
    "${CMAKE_SYSROOT}/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"
    CACHE FILEPATH "Path to the xdg-shell.xml protocol definition")

  # Output path for generated files
  set(GENERATED_WAYLAND_DIR "${CMAKE_BINARY_DIR}/generated-wayland")
  file(MAKE_DIRECTORY "${GENERATED_WAYLAND_DIR}")

  # Generated files
  set(XDG_SHELL_HEADER "${GENERATED_WAYLAND_DIR}/xdg-shell-client-protocol.h")
  set(XDG_SHELL_CODE   "${GENERATED_WAYLAND_DIR}/xdg-shell-protocol.c")

  # Generate header
  add_custom_command(
    OUTPUT ${XDG_SHELL_HEADER}
    COMMAND ${WAYLAND_SCANNER_EXECUTABLE} client-header ${XDG_SHELL_XML_PATH} ${XDG_SHELL_HEADER}
    DEPENDS ${XDG_SHELL_XML_PATH}
    COMMENT "Generating xdg-shell-client-protocol.h"
  )

  # Generate code
  add_custom_command(
    OUTPUT ${XDG_SHELL_CODE}
    COMMAND ${WAYLAND_SCANNER_EXECUTABLE} private-code ${XDG_SHELL_XML_PATH} ${XDG_SHELL_CODE}
    DEPENDS ${XDG_SHELL_XML_PATH}
    COMMENT "Generating xdg-shell-protocol.c"
  )

  # Mark files as generated
  set_source_files_properties(${XDG_SHELL_HEADER} ${XDG_SHELL_CODE} PROPERTIES GENERATED TRUE)

  # Build a static library for the generated files
  add_library(lv_xdg_shell STATIC ${XDG_SHELL_HEADER} ${XDG_SHELL_CODE})

  target_include_directories(lv_xdg_shell INTERFACE ${GENERATED_WAYLAND_DIR})
  target_link_libraries(lvgl PRIVATE lv_xdg_shell)

endif()