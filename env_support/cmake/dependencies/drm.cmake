# ============================================================
# libdrm Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "libdrm")
set(PKG_CONFIG_NAME "libdrm")
set(PKG_LIB_PRIVATE "-ldrm")

option(LV_USE_FIND_PACKAGE_LIBDRM "Resolve libdrm via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_LIBDRM "Resolve libdrm via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_LIBDRM)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(libdrm_FOUND)
    message(STATUS "lvgl: libdrm: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      libdrm::libdrm
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_LIBDRM AND PkgConfig_FOUND)
  pkg_check_modules(LIBDRM IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBDRM_FOUND)
    message(STATUS "lvgl: libdrm: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::LIBDRM
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libdrm not found. Please install libdrm.")
