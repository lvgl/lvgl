option(LV_USE_FIND_PACKAGE_FREETYPE "Resolve Freetype via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_FREETYPE "Resolve Freetype via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_FREETYPE "Fetch Freetype from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_FREETYPE)
  find_package(Freetype QUIET)
  if(Freetype_FOUND)
    message(STATUS "lvgl: Freetype: found via find_package")
    target_link_libraries(lvgl PRIVATE Freetype::Freetype)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_FREETYPE AND PkgConfig_FOUND)
  pkg_check_modules(LIBFREETYPE IMPORTED_TARGET QUIET freetype2)
  if(LIBFREETYPE_FOUND)
    message(STATUS "lvgl: Freetype: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBFREETYPE)
    return()
  endif()
endif()

if(NOT LV_FETCH_FREETYPE)
  message(
    FATAL_ERROR
      "lvgl: Freetype not found. Enable LV_FETCH_FREETYPE or install it manually."
  )
endif()

message(STATUS "lvgl: Freetype: fetching from source")
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-14-3)

set(FT_DISABLE_ZLIB
    OFF
    CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2
    ON
    CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG
    ON
    CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ
    ON
    CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI
    ON
    CACHE BOOL "" FORCE)
set(FT_WITH_ZLIB
    ON
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(freetype)

target_link_libraries(lvgl PRIVATE freetype)
