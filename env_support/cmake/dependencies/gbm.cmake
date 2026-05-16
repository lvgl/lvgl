option(LV_USE_FIND_PACKAGE_GBM "Resolve gbm via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GBM "Resolve gbm via pkg-config" ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_GBM)
  find_package(gbm QUIET)
  if(gbm_FOUND)
    message(STATUS "lvgl: gbm: found via find_package")
    target_link_libraries(lvgl PRIVATE gbm::gbm)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GBM AND PkgConfig_FOUND)
  pkg_check_modules(LIBGBM IMPORTED_TARGET QUIET gbm)
  if(LIBGBM_FOUND)
    message(STATUS "lvgl: gbm: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBGBM)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: gbm not found. Please install gbm.")
