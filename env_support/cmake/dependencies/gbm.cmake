# ============================================================
# GBM Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "gbm")
set(PKG_CONFIG_NAME "gbm")
set(PKG_LIB_PRIVATE "-lgbm")

option(LV_USE_FIND_PACKAGE_GBM "Resolve gbm via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GBM "Resolve gbm via pkg-config" ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_GBM)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(gbm_FOUND)
    message(STATUS "lvgl: gbm: found via find_package")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      gbm::gbm
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GBM AND PkgConfig_FOUND)
  pkg_check_modules(LIBGBM IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBGBM_FOUND)
    message(STATUS "lvgl: gbm: found via pkg-config")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      PkgConfig::LIBGBM
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: gbm not found. Please install gbm.")
