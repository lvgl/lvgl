option(LV_FETCH_ZLIB "Fetch zlib automatically if not found" ${LV_FETCH_DEPS})

find_package(ZLIB QUIET)

if(ZLIB_FOUND)
  message(STATUS "lvgl: zlib: found via find_package")
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(ZLIB IMPORTED_TARGET QUIET zlib)
  if(ZLIB_FOUND)
    message(STATUS "lvgl: zlib: found via pkg-config")
    return()
  endif()
endif()

if(NOT LV_FETCH_ZLIB)
  message(
    FATAL_ERROR
      "lvgl: zlib not found and LV_FETCH_ZLIB is off. Enable LV_FETCH_ZLIB or install zlib manually."
  )
endif()

message(STATUS "lvgl: zlib: fetching from source")

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG v1.3.1)

FetchContent_MakeAvailable(zlib)
