option(LV_USE_FIND_PACKAGE_X11 "Resolve X11 via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_X11 "Resolve X11 via pkg-config" ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_X11)
  find_package(X11 QUIET)
  if(X11_FOUND)
    message(STATUS "lvgl: X11: found via find_package")
    # X11::X11 imported target requires CMake 3.14+
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.14")
      target_link_libraries(lvgl PRIVATE X11::X11)
    else()
      target_link_libraries(lvgl PRIVATE ${X11_LIBRARIES})
      target_include_directories(lvgl PRIVATE ${X11_INCLUDE_DIR})
    endif()
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_X11 AND PkgConfig_FOUND)
  pkg_check_modules(X11 IMPORTED_TARGET QUIET x11)
  if(X11_FOUND)
    message(STATUS "lvgl: X11: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::X11)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: X11 not found. Please install libx11.")
