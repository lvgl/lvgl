find_package(PNG QUIET)

if(PNG_FOUND)
  message(STATUS "lvgl: libpng: found via find_package")
  target_link_libraries(lvgl PRIVATE PNG::PNG)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(LIBPNG IMPORTED_TARGET QUIET libpng)
  if(LIBPNG_FOUND)
    message(STATUS "lvgl: libpng: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBPNG)
    return()
  endif()
endif()

message(STATUS "lvgl: libpng: fetching from source")

# zlib is required by libpng — resolve it first
message(STATUS "lvgl: libpng: zlib is required by libpng. Fetching it first")

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
