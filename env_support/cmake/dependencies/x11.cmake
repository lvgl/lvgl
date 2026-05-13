# ============================================================
# X11 Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "X11")
set(PKG_CONFIG_NAME "x11")
set(PKG_LIB_PRIVATE "-lX11")

option(LV_USE_FIND_PACKAGE_X11 "Resolve X11 via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_X11 "Resolve X11 via pkg-config" ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_X11)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(X11_FOUND)
    message(STATUS "lvgl: X11: found via find_package")
    # X11::X11 imported target requires CMake 3.14+
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.14")
      lvgl_link_libraries(
        PRIVATE
        TARGETS
        X11::X11
        CMAKE_PACKAGE
        ${CMAKE_PACKAGE_NAME}
        PKG_CONFIG
        ${PKG_CONFIG_NAME}
        PKG_LIB_PRIVATE
        ${PKG_LIB_PRIVATE})
    else()
      target_link_libraries(lvgl PRIVATE ${X11_LIBRARIES})
      target_include_directories(lvgl PRIVATE ${X11_INCLUDE_DIR})
    endif()
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_X11 AND PkgConfig_FOUND)
  pkg_check_modules(X11 IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(X11_FOUND)
    message(STATUS "lvgl: X11: found via pkg-config")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      PkgConfig::X11
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: X11 not found. Please install libx11.")
