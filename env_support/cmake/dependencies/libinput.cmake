option(LV_USE_FIND_PACKAGE_LIBINPUT "Resolve libinput via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_LIBINPUT "Resolve libinput via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_LIBINPUT)
  find_package(libinput QUIET)
  if(libinput_FOUND)
    message(STATUS "lvgl: libinput: found via find_package")
    target_link_libraries(lvgl PRIVATE libinput::libinput)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_LIBINPUT AND PkgConfig_FOUND)
  pkg_check_modules(LIBINPUT IMPORTED_TARGET QUIET libinput)
  if(LIBINPUT_FOUND)
    message(STATUS "lvgl: libinput: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBINPUT)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libinput not found. Please install libinput.")
