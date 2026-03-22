option(LV_FETCH_WEBP
       "Fetch libwebp automatically if not found via find_package"
       ${LV_FETCH_DEPENDENCIES})

find_package(WebP QUIET)
if(WebP_FOUND)
  message(STATUS "libwebp: found via find_package")
  target_link_libraries(lvgl PRIVATE WebP::webp)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(WebP IMPORTED_TARGET QUIET libwebp)
  if(WebP_FOUND)
    message(STATUS "libwebp: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::WebP)
    return()
  endif()
endif()

if(NOT LV_FETCH_DEPENDENCIES OR NOT LV_FETCH_WEBP)
  message(
    FATAL_ERROR
      "libwebp not found. Enable LV_FETCH_WEBP or install it manually.")
endif()

message(STATUS "libwebp: fetching from source")
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
  GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)

FetchContent_MakeAvailable(webp)
