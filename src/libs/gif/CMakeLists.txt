set(LVGL_LIBS_GIF_SOURCES 
    gifdec.c
    lv_gif.c
)

# Create the main static library for this directory
add_library(lvgl_libs_gif STATIC ${LVGL_LIBS_GIF_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_gif PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
