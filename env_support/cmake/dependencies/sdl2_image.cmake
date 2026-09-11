# ============================================================
# SDL2_image Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "SDL2_image")
set(PKG_CONFIG_NAME "SDL2_image")
set(PKG_LIB_PRIVATE "-lSDL2_image")

option(LV_USE_FIND_PACKAGE_SDL2_IMAGE "Resolve SDL2_image via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_SDL2_IMAGE "Resolve SDL2_image via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_SDL2_IMAGE "Fetch SDL2_image from source"
       ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_SDL2_IMAGE)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(SDL2_image_FOUND AND TARGET SDL2::SDL2_image)
    message(STATUS "lvgl: SDL2_image: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      SDL2::SDL2_image
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_SDL2_IMAGE AND PkgConfig_FOUND)
  pkg_check_modules(SDL2_IMAGE IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(SDL2_IMAGE_FOUND)
    message(STATUS "lvgl: SDL2_image: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::SDL2_IMAGE
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_SDL2_IMAGE)
  message(
    FATAL_ERROR
      "lvgl: SDL2_image not found. Enable LV_FETCH_SDL2_IMAGE or install libsdl2-image manually."
  )
endif()

message(STATUS "lvgl: SDL2_image: fetching from source")
FetchContent_Declare(
  SDL2_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
  GIT_TAG release-2.8.4)

set(SDL2IMAGE_SAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(SDL2IMAGE_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(SDL2IMAGE_INSTALL
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL2_image)
lvgl_link_fetched(TARGETS SDL2_image::SDL2_image-static PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
