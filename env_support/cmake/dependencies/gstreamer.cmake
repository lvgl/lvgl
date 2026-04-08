option(LV_USE_FIND_PACKAGE_GSTREAMER "Resolve GStreamer via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GSTREAMER "Resolve GStreamer via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_GSTREAMER)
  find_package(GStreamer QUIET COMPONENTS gstreamer gstreamer-video
                                          gstreamer-app)
  if(GStreamer_FOUND
     AND GStreamer_gstreamer-video_FOUND
     AND GStreamer_gstreamer-app_FOUND)
    message(STATUS "lvgl: GStreamer: found via find_package")
    target_link_libraries(
      lvgl PRIVATE GStreamer::gstreamer GStreamer::gstreamer-video
                   GStreamer::gstreamer-app)
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
    target_link_libraries(
      lvgl PRIVATE PkgConfig::GSTREAMER PkgConfig::GSTREAMER_VIDEO
                   PkgConfig::GSTREAMER_APP)
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: GStreamer not found. Please install gstreamer-1.0, gstreamer-video-1.0 and gstreamer-app-1.0."
)
