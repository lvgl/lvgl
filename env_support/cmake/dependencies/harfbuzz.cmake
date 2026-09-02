# ============================================================
# HarfBuzz Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "harfbuzz")
set(PKG_CONFIG_NAME "harfbuzz")
set(PKG_LIB_PRIVATE "-lharfbuzz")

option(LV_USE_FIND_PACKAGE_HARFBUZZ "Resolve HarfBuzz via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_HARFBUZZ "Resolve HarfBuzz via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_HARFBUZZ "Fetch HarfBuzz from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_HARFBUZZ)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(harfbuzz_FOUND)
    message(STATUS "lvgl: HarfBuzz: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      harfbuzz::harfbuzz
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_HARFBUZZ AND PkgConfig_FOUND)
  pkg_check_modules(LIBHARFBUZZ IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBHARFBUZZ_FOUND)
    message(STATUS "lvgl: HarfBuzz: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::LIBHARFBUZZ
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_HARFBUZZ)
  message(
    FATAL_ERROR
      "lvgl: HarfBuzz not found. Enable LV_FETCH_HARFBUZZ or install it manually."
  )
endif()

message(STATUS "lvgl: HarfBuzz: fetching from source")
FetchContent_Declare(
  harfbuzz
  GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
  GIT_TAG 11.2.1
  GIT_SHALLOW TRUE)
set(HB_BUILD_SUBSET
    OFF
    CACHE BOOL "" FORCE)
set(HB_HAVE_FREETYPE
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(harfbuzz)
lvgl_link_fetched(TARGETS harfbuzz PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
