# ============================================================
# FFmpeg Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "FFmpeg")
set(PKG_CONFIG_NAME "libavcodec" "libavformat" "libavutil" "libswscale")
set(PKG_LIB_PRIVATE "-lavcodec -lavformat -lavutil -lswscale")

option(LV_USE_FIND_PACKAGE_FFMPEG "Resolve FFmpeg via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_FFMPEG "Resolve FFmpeg via pkg-config"
       ${LV_USE_PKG_CONFIG})

if(LV_USE_FIND_PACKAGE_FFMPEG)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(FFmpeg_FOUND)
    message(STATUS "lvgl: FFmpeg: found via find_package")
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      FFmpeg::FFmpeg
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
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
    lvgl_link_libraries(
      PRIVATE
      TARGETS
      PkgConfig::AVFORMAT
      PkgConfig::AVCODEC
      PkgConfig::AVUTIL
      PkgConfig::SWSCALE
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})

    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: FFmpeg not found. Please install libavformat, libavcodec, libavutil and libswscale."
)
