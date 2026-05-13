# ============================================================
# libinput Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "libinput")
set(PKG_CONFIG_NAME "libinput")
set(PKG_LIB_PRIVATE "-linput")

option(LV_USE_FIND_PACKAGE_LIBINPUT "Resolve libinput via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_LIBINPUT "Resolve libinput via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_LIBINPUT)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(libinput_FOUND)
    message(STATUS "lvgl: libinput: found via find_package")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      libinput::libinput
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_LIBINPUT AND PkgConfig_FOUND)
  pkg_check_modules(LIBINPUT IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBINPUT_FOUND)
    message(STATUS "lvgl: libinput: found via pkg-config")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      PkgConfig::LIBINPUT
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libinput not found. Please install libinput.")
