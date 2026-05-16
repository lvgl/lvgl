option(LV_USE_FIND_PACKAGE_ZLIB "Resolve zlib via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_ZLIB "Resolve zlib via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_ZLIB "Fetch zlib from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_ZLIB)
  find_package(ZLIB QUIET)
  if(ZLIB_FOUND)
    message(STATUS "lvgl: zlib: found via find_package")
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_ZLIB AND PkgConfig_FOUND)
  pkg_check_modules(ZLIB IMPORTED_TARGET QUIET zlib)
  if(ZLIB_FOUND)
    message(STATUS "lvgl: zlib: found via pkg-config")
    return()
  endif()
endif()

if(NOT LV_FETCH_ZLIB)
  message(
    FATAL_ERROR
      "lvgl: zlib not found. Enable LV_FETCH_ZLIB or install it manually.")
endif()

message(STATUS "lvgl: zlib: fetching from source")
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG v1.3.1)

FetchContent_MakeAvailable(zlib)
