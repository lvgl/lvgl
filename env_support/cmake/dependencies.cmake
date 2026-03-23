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
