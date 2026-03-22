find_package(gbm QUIET)

if(gbm_FOUND)
  message(STATUS "lvgl: gbm: found via find_package")
  target_link_libraries(lvgl PRIVATE gbm::gbm)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(LIBGBM IMPORTED_TARGET QUIET gbm)
  if(LIBGBM_FOUND)
    message(STATUS "lvgl: gbm: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBGBM)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: gbm not found. Please install gbm.")
