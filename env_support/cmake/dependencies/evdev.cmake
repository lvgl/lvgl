option(LV_USE_FIND_PACKAGE_EVDEV "Resolve libevdev via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_EVDEV "Resolve libevdev via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_EVDEV)
  find_package(evdev QUIET)
  if(evdev_FOUND)
    message(STATUS "lvgl: evdev: found via find_package")
    target_link_libraries(lvgl PRIVATE evdev::evdev)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_EVDEV AND PkgConfig_FOUND)
  pkg_check_modules(EVDEV IMPORTED_TARGET QUIET libevdev)
  if(EVDEV_FOUND)
    message(STATUS "lvgl: evdev: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::EVDEV)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libevdev not found. Please install libevdev.")
