# ============================================================
# OpenGL ES v2 Configuration
# ============================================================
set(PKG_CONFIG_NAME "glesv2")
set(PKG_LIB_PRIVATE "-lGLESv2")

option(LV_USE_FIND_PACKAGE_GLESV2 "Resolve OpenGL ES via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GLESV2 "Resolve OpenGL ES via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_GLESV2)
  find_package(OpenGL QUIET COMPONENTS GLES2)
  if(TARGET OpenGL::GLES2)
    message(STATUS "lvgl: OpenGL ES: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      OpenGL::GLES2
      CMAKE_PACKAGE
      OpenGL
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GLESV2 AND PkgConfig_FOUND)
  pkg_check_modules(GLESV2 IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(GLESV2_FOUND)
    message(STATUS "lvgl: OpenGL ES: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::GLESV2
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: OpenGL ES not found. Please install libgles")
