# ============================================================
# Freetype Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "Freetype")
set(PKG_CONFIG_NAME "freetype2")
set(PKG_LIB_PRIVATE "-lfreetype")

option(LV_USE_FIND_PACKAGE_FREETYPE "Resolve Freetype via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_FREETYPE "Resolve Freetype via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_FREETYPE "Fetch Freetype from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_FREETYPE)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(Freetype_FOUND)
    message(STATUS "lvgl: Freetype: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      Freetype::Freetype
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_FREETYPE AND PkgConfig_FOUND)
  pkg_check_modules(LIBFREETYPE IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBFREETYPE_FOUND)
    message(STATUS "lvgl: Freetype: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::LIBFREETYPE
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
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
lvgl_link_fetched(TARGETS freetype PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
