option(LV_USE_FIND_PACKAGE_FFMPEG "Resolve FFmpeg via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_FFMPEG "Resolve FFmpeg via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_FFMPEG)
  find_package(FFmpeg QUIET)
  if(FFmpeg_FOUND)
    message(STATUS "lvgl: FFmpeg: found via find_package")
    target_link_libraries(lvgl PRIVATE FFmpeg::FFmpeg)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_FFMPEG AND PkgConfig_FOUND)
  pkg_check_modules(AVFORMAT IMPORTED_TARGET QUIET libavformat)
  pkg_check_modules(AVCODEC IMPORTED_TARGET QUIET libavcodec)
  pkg_check_modules(AVUTIL IMPORTED_TARGET QUIET libavutil)
  pkg_check_modules(SWSCALE IMPORTED_TARGET QUIET libswscale)
  if(AVFORMAT_FOUND
     AND AVCODEC_FOUND
     AND AVUTIL_FOUND
     AND SWSCALE_FOUND)
    message(STATUS "lvgl: FFmpeg: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::AVFORMAT PkgConfig::AVCODEC
                                       PkgConfig::AVUTIL PkgConfig::SWSCALE)
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: FFmpeg not found. Please install libavformat, libavcodec, libavutil and libswscale."
)
