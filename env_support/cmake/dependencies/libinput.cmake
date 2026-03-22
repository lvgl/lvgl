find_package(libinput QUIET)

if(libinput_FOUND)
  message(STATUS "lvgl: libinput: found via find_package")
  target_link_libraries(lvgl PRIVATE libinput::libinput)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(LIBINPUT IMPORTED_TARGET QUIET libinput)
  if(LIBINPUT_FOUND)
    message(STATUS "lvgl: libinput: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBINPUT)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libinput not found. Please install libinput.")
