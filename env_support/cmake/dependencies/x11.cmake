find_package(X11 QUIET)

if(X11_FOUND)
  message(STATUS "lvgl: X11: found via find_package")
  target_link_libraries(lvgl PRIVATE X11::X11)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(X11 IMPORTED_TARGET QUIET x11)
  if(X11_FOUND)
    message(STATUS "lvgl: X11: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::X11)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: X11 not found. Please install libx11.")
