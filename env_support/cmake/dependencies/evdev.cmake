# ============================================================
# libevdev Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "evdev")
set(PKG_CONFIG_NAME "libevdev")
set(PKG_LIB_PRIVATE "-levdev")

option(LV_USE_FIND_PACKAGE_EVDEV "Resolve libevdev via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_EVDEV "Resolve libevdev via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_EVDEV)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(evdev_FOUND)
    message(STATUS "lvgl: evdev: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      evdev::evdev
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_EVDEV AND PkgConfig_FOUND)
  pkg_check_modules(EVDEV IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(EVDEV_FOUND)
    message(STATUS "lvgl: evdev: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::EVDEV
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libevdev not found. Please install libevdev.")
