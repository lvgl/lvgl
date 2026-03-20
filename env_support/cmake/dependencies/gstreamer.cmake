find_package(GStreamer QUIET)

if(GStreamer_FOUND)
  message(STATUS "lvgl: GStreamer: found via find_package")
  target_link_libraries(lvgl PRIVATE GStreamer::GStreamer)
  return()
endif()

if(PkgConfig_FOUND)
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
