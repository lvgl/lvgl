# ============================================================
# SDL2 Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "SDL2")
set(PKG_CONFIG_NAME "sdl2")
set(PKG_LIB_PRIVATE "-lSDL2")

option(LV_USE_FIND_PACKAGE_SDL2 "Resolve SDL2 via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_SDL2 "Resolve SDL2 via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_SDL2 "Fetch SDL2 from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_SDL2)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(SDL2_FOUND AND TARGET SDL2::SDL2)
    message(STATUS "lvgl: SDL2: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      SDL2::SDL2
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_SDL2 AND PkgConfig_FOUND)
  pkg_check_modules(SDL2 IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(SDL2_FOUND)
    message(STATUS "lvgl: SDL2: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::SDL2
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_SDL2)
  message(
    FATAL_ERROR
      "lvgl: SDL2 not found. Enable LV_FETCH_SDL2 or install libsdl2 manually.")
endif()

message(STATUS "lvgl: SDL2: fetching from source")
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG release-2.30.11)

set(SDL_OPENGLES
    ON
    CACHE BOOL "" FORCE)
set(SDL_STATIC
    ON
    CACHE BOOL "" FORCE)
set(SDL_X11
    ON
    CACHE BOOL "" FORCE)
set(SDL_WAYLAND
    ON
    CACHE BOOL "" FORCE)
set(SDL_KMSDRM
    ON
    CACHE BOOL "" FORCE)

set(SDL_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(SDL_TEST
    OFF
    CACHE BOOL "" FORCE)
set(SDL_ALSA
    OFF
    CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE
    OFF
    CACHE BOOL "" FORCE)
set(SDL_PULSEAUDIO
    OFF
    CACHE BOOL "" FORCE)
set(SDL_JACK
    OFF
    CACHE BOOL "" FORCE)
set(SDL_OFFSCREEN
    OFF
    CACHE BOOL "" FORCE)
set(SDL_OPENGL
    OFF
    CACHE BOOL "" FORCE)
set(SDL_VULKAN
    OFF
    CACHE BOOL "" FORCE)
set(SDL_DBUS
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL2)
lvgl_link_fetched(TARGETS SDL2::SDL2-static PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
