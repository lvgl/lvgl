find_package(Wayland QUIET)

if(Wayland_FOUND)
  message(STATUS "lvgl: Wayland: found via find_package")
  set(WAYLAND_CLIENT_LIBRARIES Wayland::Client)
  set(WAYLAND_CURSOR_LIBRARIES Wayland::Cursor)
endif()

if(NOT Wayland_FOUND AND PkgConfig_FOUND)
  pkg_check_modules(WAYLAND_CLIENT IMPORTED_TARGET QUIET wayland-client)
  pkg_check_modules(WAYLAND_CURSOR IMPORTED_TARGET QUIET wayland-cursor)
  pkg_check_modules(WAYLAND_PROTOCOLS QUIET wayland-protocols>=1.20)
  if(WAYLAND_CLIENT_FOUND AND WAYLAND_CURSOR_FOUND)
    message(STATUS "lvgl: Wayland: found via pkg-config")
    set(WAYLAND_CLIENT_LIBRARIES PkgConfig::WAYLAND_CLIENT)
    set(WAYLAND_CURSOR_LIBRARIES PkgConfig::WAYLAND_CURSOR)
  else()
    message(
      FATAL_ERROR
        "lvgl: Wayland not found. Please install wayland-client and wayland-cursor."
    )
  endif()
endif()

if(NOT Wayland_FOUND AND NOT PkgConfig_FOUND)
  message(FATAL_ERROR "lvgl: Wayland not found and PkgConfig unavailable.")
endif()

# EGL
if(CONFIG_LV_WAYLAND_USE_EGL)
  pkg_check_modules(WAYLAND_EGL IMPORTED_TARGET QUIET wayland-egl)
  if(WAYLAND_EGL_FOUND)
    message(STATUS "lvgl: Wayland EGL: found via pkg-config")
    set(WAYLAND_EGL_LIBRARIES PkgConfig::WAYLAND_EGL)
  else()
    message(
      FATAL_ERROR "lvgl: wayland-egl not found. Please install wayland-egl.")
  endif()
endif()

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
  execute_process(COMMAND wayland-scanner client-header ${XDG_SHELL_XML}
                          ${XDG_SHELL_HEADER})
  execute_process(COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE}
                          ${XDG_SHELL_XML} ${XDG_SHELL_SOURCE})
endif()

set(WAYLAND_PROTOCOL_SOURCES ${XDG_SHELL_SOURCE})

# dmabuf (optional)
if(CONFIG_LV_WAYLAND_USE_G2D)
  set(DMABUF_XML "${PROTOCOL_ROOT}/stable/linux-dmabuf/linux-dmabuf-v1.xml")
  set(DMABUF_HEADER "${PROTOCOLS_DIR}/wayland_linux_dmabuf.h")
  set(DMABUF_SOURCE "${PROTOCOLS_DIR}/wayland_linux_dmabuf.c")

  if(NOT EXISTS ${DMABUF_HEADER} OR NOT EXISTS ${DMABUF_SOURCE})
    execute_process(COMMAND wayland-scanner client-header ${DMABUF_XML}
                            ${DMABUF_HEADER})
    execute_process(COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE}
                            ${DMABUF_XML} ${DMABUF_SOURCE})
  endif()

  list(APPEND WAYLAND_PROTOCOL_SOURCES ${DMABUF_SOURCE})
endif()

# Protocol library
add_library(lv_wayland_protocols ${WAYLAND_PROTOCOL_SOURCES})
target_include_directories(lv_wayland_protocols PUBLIC ${PROTOCOLS_DIR})

# Link lvgl
target_link_libraries(
  lvgl PRIVATE lv_wayland_protocols ${WAYLAND_CLIENT_LIBRARIES}
               ${WAYLAND_CURSOR_LIBRARIES})

if(CONFIG_LV_WAYLAND_USE_EGL)
  target_link_libraries(lvgl PRIVATE ${WAYLAND_EGL_LIBRARIES})
endif()
