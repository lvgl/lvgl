# ============================================================
# EGL Configuration
# ============================================================
set(PKG_CONFIG_NAME "egl")
set(PKG_LIB_PRIVATE "-lEGL")

option(LV_USE_FIND_PACKAGE_EGL "Resolve EGL via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_EGL "Resolve EGL via pkg-config" ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_EGL)
  find_package(OpenGL QUIET COMPONENTS EGL)
  if(TARGET OpenGL::EGL)
    message(STATUS "lvgl: EGL: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      OpenGL::EGL
      CMAKE_PACKAGE
      OpenGL
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_EGL AND PkgConfig_FOUND)
  pkg_check_modules(EGL IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(EGL_FOUND)
    message(STATUS "lvgl: EGL: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::EGL
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: EGL not found. Please install libegl")
