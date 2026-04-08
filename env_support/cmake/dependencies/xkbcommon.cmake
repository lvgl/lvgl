option(LV_USE_FIND_PACKAGE_XKBCOMMON "Resolve xkbcommon via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_XKBCOMMON "Resolve xkbcommon via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_XKBCOMMON)
  find_package(Xkbcommon QUIET)
  if(Xkbcommon_FOUND)
    message(STATUS "lvgl: xkbcommon: found via find_package")
    target_link_libraries(lvgl PRIVATE Xkbcommon::Xkbcommon)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_XKBCOMMON AND PkgConfig_FOUND)
  pkg_check_modules(XKBCOMMON IMPORTED_TARGET QUIET xkbcommon)
  if(XKBCOMMON_FOUND)
    message(STATUS "lvgl: xkbcommon: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::XKBCOMMON)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: xkbcommon not found. Please install xkbcommon.")
