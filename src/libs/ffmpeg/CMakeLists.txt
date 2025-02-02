set(LVGL_LIBS_FFMPEG_SOURCES 
    lv_ffmpeg.c
)

# Create the main static library for this directory
add_library(lvgl_libs_ffmpeg STATIC ${LVGL_LIBS_FFMPEG_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_ffmpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
