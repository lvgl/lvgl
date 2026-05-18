# ============================================================
# Wayland Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME Wayland)
set(PKG_CONFIG_NAME wayland-client wayland-cursor)
set(PKG_LIB_PRIVATE -lwayland-client -lwayland-cursor)
set(WAYLAND_FOUND 0)

if(CONFIG_LV_WAYLAND_USE_EGL)
  list(APPEND PKG_CONFIG_NAME wayland-egl)
  list(APPEND PKG_LIB_PRIVATE -lwayland-egl)
endif()

option(LV_USE_FIND_PACKAGE_WAYLAND "Resolve Wayland via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_WAYLAND "Resolve Wayland via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_WAYLAND)
  find_package(Wayland QUIET)
  if(Wayland_FOUND)
    set(WAYLAND_FOUND 1)
    message(STATUS "lvgl: Wayland: found via find_package")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      Wayland::Client
      Wayland::Cursor
      Wayland::Egl
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
  endif()
endif()

if(NOT WAYLAND_FOUND
   AND LV_USE_PKG_CONFIG_WAYLAND
   AND PkgConfig_FOUND)
  pkg_check_modules(WAYLAND_CLIENT IMPORTED_TARGET QUIET wayland-client)
  pkg_check_modules(WAYLAND_CURSOR IMPORTED_TARGET QUIET wayland-cursor)
  if(CONFIG_LV_WAYLAND_USE_EGL)
    pkg_check_modules(WAYLAND_EGL IMPORTED_TARGET QUIET wayland-egl)
  endif()

  if(NOT WAYLAND_CLIENT_FOUND
     OR NOT WAYLAND_CURSOR_FOUND
     OR (CONFIG_LV_WAYLAND_USE_EGL AND NOT WAYLAND_EGL_FOUND))
    message(
      FATAL_ERROR
        "lvgl: Wayland not found. Please install wayland-client and wayland-cursor."
    )
  else()
    message(STATUS "lvgl: Wayland: found via pkg-config")
    set(TARGETS PkgConfig::WAYLAND_CLIENT PkgConfig::WAYLAND_CURSOR)
    set(WAYLAND_FOUND 1)
    if(CONFIG_LV_WAYLAND_USE_EGL)
      list(APPEND TARGETS PkgConfig::WAYLAND_EGL)
    endif()
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      ${TARGETS}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
  endif()
endif()

if(NOT WAYLAND_FOUND)

  if(CONFIG_LV_WAYLAND_USE_EGL)
    message(
      FATAL_ERROR
        "lvgl: Wayland not found. Please install wayland-client, wayland-cursor and wayland-egl."
    )
  else()
    message(
      FATAL_ERROR
        "lvgl: Wayland not found. Please install wayland-client and wayland-cursor."
    )
  endif()
endif()

find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

# Protocol root resolution
if(DEFINED ENV{SDKTARGETSYSROOT} AND DEFINED ENV{SYSROOT})
  message(
    FATAL_ERROR
      "Both SDKTARGETSYSROOT and SYSROOT are set. Please set only one.")
endif()

if(DEFINED ENV{SDKTARGETSYSROOT})
  set(PROTOCOL_ROOT "$ENV{SDKTARGETSYSROOT}/usr/share/wayland-protocols")
elseif(DEFINED ENV{SYSROOT})
  set(PROTOCOL_ROOT "$ENV{SYSROOT}/usr/share/wayland-protocols")
else()
  set(PROTOCOL_ROOT "/usr/share/wayland-protocols")
endif()

if(NOT EXISTS ${PROTOCOL_ROOT})
  message(FATAL_ERROR "lvgl: Wayland protocols not found at ${PROTOCOL_ROOT}")
endif()

set(PROTOCOLS_DIR "${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols")
file(MAKE_DIRECTORY ${PROTOCOLS_DIR})
message(STATUS "lvgl: Wayland protocol directory: ${PROTOCOLS_DIR}")

if(BUILD_SHARED_LIBS)
  set(WAYLAND_SCANNER_CODE_MODE "public-code")
else()
  set(WAYLAND_SCANNER_CODE_MODE "private-code")
endif()

# xdg-shell (always)
set(XDG_SHELL_XML "${PROTOCOL_ROOT}/stable/xdg-shell/xdg-shell.xml")
set(XDG_SHELL_HEADER "${PROTOCOLS_DIR}/wayland_xdg_shell.h")
set(XDG_SHELL_SOURCE "${PROTOCOLS_DIR}/wayland_xdg_shell.c")

if(NOT EXISTS ${XDG_SHELL_HEADER} OR NOT EXISTS ${XDG_SHELL_SOURCE})
  execute_process(
    COMMAND wayland-scanner client-header ${XDG_SHELL_XML} ${XDG_SHELL_HEADER}
    RESULT_VARIABLE XDG_SHELL_HEADER_RESULT
    ERROR_VARIABLE XDG_SHELL_HEADER_ERROR)
  if(NOT XDG_SHELL_HEADER_RESULT EQUAL 0)
    message(
      FATAL_ERROR
        "lvgl: wayland-scanner failed to generate xdg-shell header: ${XDG_SHELL_HEADER_ERROR}"
    )
  endif()

  execute_process(
    COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE} ${XDG_SHELL_XML}
            ${XDG_SHELL_SOURCE}
    RESULT_VARIABLE XDG_SHELL_SOURCE_RESULT
    ERROR_VARIABLE XDG_SHELL_SOURCE_ERROR)
  if(NOT XDG_SHELL_SOURCE_RESULT EQUAL 0)
    message(
      FATAL_ERROR
        "lvgl: wayland-scanner failed to generate xdg-shell source: ${XDG_SHELL_SOURCE_ERROR}"
    )
  endif()
endif()

set(WAYLAND_PROTOCOL_SOURCES ${XDG_SHELL_SOURCE})

# dmabuf (optional)
if(CONFIG_LV_WAYLAND_USE_G2D)
  set(DMABUF_XML "${PROTOCOL_ROOT}/stable/linux-dmabuf/linux-dmabuf-v1.xml")
  set(DMABUF_HEADER "${PROTOCOLS_DIR}/wayland_linux_dmabuf.h")
  set(DMABUF_SOURCE "${PROTOCOLS_DIR}/wayland_linux_dmabuf.c")

  if(NOT EXISTS ${DMABUF_HEADER} OR NOT EXISTS ${DMABUF_SOURCE})
    execute_process(
      COMMAND wayland-scanner client-header ${DMABUF_XML} ${DMABUF_HEADER}
      RESULT_VARIABLE DMABUF_HEADER_RESULT
      ERROR_VARIABLE DMABUF_HEADER_ERROR)
    if(NOT DMABUF_HEADER_RESULT EQUAL 0)
      message(
        FATAL_ERROR
          "lvgl: wayland-scanner failed to generate dmabuf header: ${DMABUF_HEADER_ERROR}"
      )
    endif()

    execute_process(
      COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE} ${DMABUF_XML}
              ${DMABUF_SOURCE}
      RESULT_VARIABLE DMABUF_SOURCE_RESULT
      ERROR_VARIABLE DMABUF_SOURCE_ERROR)
    if(NOT DMABUF_SOURCE_RESULT EQUAL 0)
      message(
        FATAL_ERROR
          "lvgl: wayland-scanner failed to generate dmabuf source: ${DMABUF_SOURCE_ERROR}"
      )
    endif()
  endif()

  list(APPEND WAYLAND_PROTOCOL_SOURCES ${DMABUF_SOURCE})
endif()

# Protocol library
add_library(lv_wayland_protocols OBJECT ${WAYLAND_PROTOCOL_SOURCES})
target_include_directories(lv_wayland_protocols PUBLIC ${PROTOCOLS_DIR})

# Instead of target_link_libraries which creates an exportable dependency
target_sources(lvgl PRIVATE $<TARGET_OBJECTS:lv_wayland_protocols>)
target_include_directories(
  lvgl
  PRIVATE $<TARGET_PROPERTY:lv_wayland_protocols,INTERFACE_INCLUDE_DIRECTORIES>)
