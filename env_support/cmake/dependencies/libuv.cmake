# ============================================================
# libuv Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "libuv")
set(PKG_CONFIG_NAME "libuv")
set(PKG_LIB_PRIVATE "-luv")

option(LV_USE_FIND_PACKAGE_LIBUV "Resolve libuv via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_LIBUV "Resolve libuv via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_LIBUV)
  find_package(${CMAKE_PACKAGE_NAME} CONFIG QUIET)
  # libuv exports the shared library as libuv::uv and the static one as
  # libuv::uv_a. Which of them exists depends on how it was built.
  foreach(_target libuv::uv_a libuv::uv)
    if(TARGET ${_target})
      message(STATUS "lvgl: libuv: found via find_package")
      lvgl_link_packages(
        PRIVATE
        TARGETS
        ${_target}
        CMAKE_PACKAGE
        ${CMAKE_PACKAGE_NAME}
        PKG_CONFIG
        ${PKG_CONFIG_NAME}
        PKG_LIB_PRIVATE
        ${PKG_LIB_PRIVATE})
      return()
    endif()
  endforeach()
endif()

if(LV_USE_PKG_CONFIG_LIBUV AND PkgConfig_FOUND)
  pkg_check_modules(LIBUV IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBUV_FOUND)
    message(STATUS "lvgl: libuv: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::LIBUV
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: libuv not found. Please install libuv.")
