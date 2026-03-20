find_package(SDL2 QUIET)
if(SDL2_FOUND)
  message(STATUS "lvgl: SDL2: found via find_package")
  target_link_libraries(lvgl PRIVATE SDL2::SDL2 SDL2::SDL2_image)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(SDL2 IMPORTED_TARGET QUIET sdl2)
  pkg_check_modules(SDL2_IMAGE IMPORTED_TARGET QUIET SDL2_image)
  if(SDL2_FOUND AND SDL2_IMAGE_FOUND)
    message(STATUS "lvgl: SDL2: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::SDL2 PkgConfig::SDL2_IMAGE)
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: SDL2 or SDL2_image not found. Please install libsdl2 and libsdl2-image."
)
