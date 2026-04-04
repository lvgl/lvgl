option(LV_USE_FIND_PACKAGE_SDL2_IMAGE "Resolve SDL2_image via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_SDL2_IMAGE "Resolve SDL2_image via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_SDL2_IMAGE "Fetch SDL2_image from source"
       ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_SDL2_IMAGE)
  find_package(SDL2_image QUIET)
  if(SDL2_image_FOUND AND TARGET SDL2::SDL2_image)
    message(STATUS "lvgl: SDL2_image: found via find_package")
    target_link_libraries(lvgl PRIVATE SDL2::SDL2_image)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_SDL2_IMAGE AND PkgConfig_FOUND)
  pkg_check_modules(SDL2_IMAGE IMPORTED_TARGET QUIET SDL2_image)
  if(SDL2_IMAGE_FOUND)
    message(STATUS "lvgl: SDL2_image: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::SDL2_IMAGE)
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
target_link_libraries(lvgl PRIVATE SDL2_image::SDL2_image-static)
