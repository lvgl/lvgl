option(LV_USE_FIND_PACKAGE_WEBP "Resolve libwebp via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_WEBP "Resolve libwebp via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_WEBP "Fetch libwebp from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_WEBP)
  find_package(WebP QUIET)
  if(WebP_FOUND)
    message(STATUS "lvgl: libwebp: found via find_package")
    target_link_libraries(lvgl PRIVATE WebP::webp)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_WEBP AND PkgConfig_FOUND)
  pkg_check_modules(WebP IMPORTED_TARGET QUIET libwebp)
  if(WebP_FOUND)
    message(STATUS "lvgl: libwebp: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::WebP)
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
target_link_libraries(lvgl PRIVATE webp)
