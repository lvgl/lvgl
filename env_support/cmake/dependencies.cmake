include(FetchContent)

set(FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.deps"
    CACHE PATH "Directory for fetched dependencies" FORCE)

find_package(PkgConfig)

if(UNIX AND NOT PkgConfig_FOUND)
  message(
    WARNING
      "pkg-config not found - system libraries will only be resolved via find_package. "
      "Install `pkg-config` to improve dependency detection.")
endif()

# ====== Draw Units ====== #
if(CONFIG_LV_USE_DRAW_G2D)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/g2d.cmake)
endif()

if(CONFIG_LV_USE_DRAW_SDL)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl2_image.cmake)
endif()

# ====== Drivers ====== #
if(CONFIG_LV_USE_SDL)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/sdl2.cmake)
endif()

if(CONFIG_LV_USE_LINUX_DRM)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/drm.cmake)
endif()

if(CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS OR CONFIG_LV_LINUX_DRM_USE_EGL)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/gbm.cmake)
endif()

if(CONFIG_LV_USE_WAYLAND)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/wayland.cmake)
endif()

if(CONFIG_LV_USE_X11)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/x11.cmake)
endif()

if(CONFIG_LV_USE_GLFW)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/glfw.cmake)
endif()

# ====== Indev ====== #
if(CONFIG_LV_USE_WAYLAND OR CONFIG_LV_USE_LIBINPUT)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/xkbcommon.cmake)
endif()

if(CONFIG_LV_USE_EVDEV)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/evdev.cmake)
endif()

if(CONFIG_LV_USE_LIBINPUT)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/libinput.cmake)
endif()

# ====== Image ====== #
if(CONFIG_LV_USE_LIBWEBP OR CONFIG_LV_USE_GLTF)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/webp.cmake)
endif()

# ====== Video ====== #
if(CONFIG_LV_USE_GSTREAMER)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/gstreamer.cmake)
endif()

if(CONFIG_LV_USE_FFMPEG)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg.cmake)
endif()

# ====== Image ====== #
if(CONFIG_LV_USE_LIBJPEG_TURBO)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/jpeg.cmake)
endif()

if(CONFIG_LV_USE_LIBPNG)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/png.cmake)
endif()

# ====== Font ====== #
if(CONFIG_LV_USE_FREETYPE)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/freetype.cmake)
endif()

# ====== Libraries ====== #
if(CONFIG_LV_USE_GLTF)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/fastgltf.cmake)
endif()

# ====== External Demos  ====== #
if(CONFIG_LV_USE_DEMO_FLEX_LAYOUT
   OR CONFIG_LV_USE_DEMO_MULTILANG
   OR CONFIG_LV_USE_DEMO_TRANSFORM
   OR CONFIG_LV_USE_DEMO_SCROLL
   OR CONFIG_LV_USE_DEMO_EBIKE
   OR CONFIG_LV_USE_DEMO_HIGH_RES
   OR CONFIG_LV_USE_DEMO_SMARTWATCH)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/lv_demos_ext.cmake)
endif()
