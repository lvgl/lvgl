# ============================================================
# libwebp Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "WebP")
set(PKG_CONFIG_NAME "libwebp")
set(PKG_LIB_PRIVATE "-lwebp")

option(LV_USE_FIND_PACKAGE_WEBP "Resolve libwebp via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_WEBP "Resolve libwebp via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_WEBP "Fetch libwebp from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_WEBP)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(WebP_FOUND)
    message(STATUS "lvgl: libwebp: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      WebP::webp
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_WEBP AND PkgConfig_FOUND)
  pkg_check_modules(WebP IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(WebP_FOUND)
    message(STATUS "lvgl: libwebp: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::WebP
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_WEBP)
  message(
    FATAL_ERROR
      "lvgl: libwebp not found. Enable LV_FETCH_WEBP or install it manually.")
endif()

message(STATUS "lvgl: libwebp: fetching from source")
set(WEBP_BUILD_ANIM_UTILS
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_CWEBP
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_DWEBP
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_GIF2WEBP
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_IMG2WEBP
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_VWEBP
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_WEBPINFO
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_WEBPMUX
    OFF
    CACHE BOOL "" FORCE)
set(WEBP_BUILD_EXTRAS
    OFF
    CACHE BOOL "" FORCE)

FetchContent_Declare(
  webp
  GIT_REPOSITORY https://github.com/webmproject/libwebp
  GIT_TAG 4fa21912338357f89e4fd51cf2368325b59e9bd9)

FetchContent_MakeAvailable(webp)
lvgl_link_fetched(TARGETS webp PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
