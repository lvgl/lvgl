option(LV_USE_FIND_PACKAGE_PNG "Resolve libpng via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_PNG "Resolve libpng via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_PNG "Fetch libpng from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_PNG)
  find_package(PNG QUIET)
  if(PNG_FOUND)
    message(STATUS "lvgl: libpng: found via find_package")
    target_link_libraries(lvgl PRIVATE PNG::PNG)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_PNG AND PkgConfig_FOUND)
  pkg_check_modules(LIBPNG IMPORTED_TARGET QUIET libpng)
  if(LIBPNG_FOUND)
    message(STATUS "lvgl: libpng: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBPNG)
    return()
  endif()
endif()

if(NOT LV_FETCH_PNG)
  message(
    FATAL_ERROR
      "lvgl: libpng not found. Enable LV_FETCH_PNG or install it manually.")
endif()

message(STATUS "lvgl: libpng: fetching from source")
message(STATUS "lvgl: libpng: zlib is required, resolving it first")
include("${CMAKE_CURRENT_LIST_DIR}/zlib.cmake")

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng
  GIT_TAG v1.6.47)

set(PNG_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(PNG_STATIC
    ON
    CACHE BOOL "" FORCE)
set(PNG_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(PNG_TOOLS
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libpng)
target_link_libraries(lvgl PRIVATE png_static)
