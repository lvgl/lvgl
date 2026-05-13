# ============================================================
# xkbcommon Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "Xkbcommon")
set(PKG_CONFIG_NAME "xkbcommon")
set(PKG_LIB_PRIVATE "-lxkbcommon")

option(LV_USE_FIND_PACKAGE_XKBCOMMON "Resolve xkbcommon via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_XKBCOMMON "Resolve xkbcommon via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_XKBCOMMON)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(Xkbcommon_FOUND)
    message(STATUS "lvgl: xkbcommon: found via find_package")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      Xkbcommon::Xkbcommon
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_XKBCOMMON AND PkgConfig_FOUND)
  pkg_check_modules(XKBCOMMON IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(XKBCOMMON_FOUND)
    message(STATUS "lvgl: xkbcommon: found via pkg-config")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      PkgConfig::XKBCOMMON
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: xkbcommon not found. Please install xkbcommon.")
