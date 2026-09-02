# ============================================================
# GStreamer Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "GStreamer")
set(PKG_CONFIG_NAME "gstreamer-1.0" "gstreamer-video-1.0" "gstreamer-app-1.0")
set(PKG_LIB_PRIVATE "-lgstreamer-1.0 -lgstvideo-1.0 -lgstapp-1.0")

option(LV_USE_FIND_PACKAGE_GSTREAMER "Resolve GStreamer via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GSTREAMER "Resolve GStreamer via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_GSTREAMER)
  find_package(${CMAKE_PACKAGE_NAME} QUIET COMPONENTS gstreamer gstreamer-video
                                                      gstreamer-app)
  if(GStreamer_FOUND
     AND GStreamer_gstreamer-video_FOUND
     AND GStreamer_gstreamer-app_FOUND)
    message(STATUS "lvgl: GStreamer: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      GStreamer::gstreamer
      GStreamer::gstreamer-video
      GStreamer::gstreamer-app
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GSTREAMER AND PkgConfig_FOUND)
  pkg_check_modules(GSTREAMER IMPORTED_TARGET QUIET gstreamer-1.0)
  pkg_check_modules(GSTREAMER_VIDEO IMPORTED_TARGET QUIET gstreamer-video-1.0)
  pkg_check_modules(GSTREAMER_APP IMPORTED_TARGET QUIET gstreamer-app-1.0)
  if(GSTREAMER_FOUND
     AND GSTREAMER_VIDEO_FOUND
     AND GSTREAMER_APP_FOUND)
    message(STATUS "lvgl: GStreamer: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::GSTREAMER
      PkgConfig::GSTREAMER_VIDEO
      PkgConfig::GSTREAMER_APP
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: GStreamer not found. Please install gstreamer-1.0, gstreamer-video-1.0 and gstreamer-app-1.0."
)
