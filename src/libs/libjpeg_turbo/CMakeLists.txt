set(LVGL_LIBS_LIBJPEG_TURBO_SOURCES 
    lv_libjpeg_turbo.c
)

# Create the main static library for this directory
add_library(lvgl_libs_libjpeg_turbo STATIC ${LVGL_LIBS_LIBJPEG_TURBO_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_libjpeg_turbo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
