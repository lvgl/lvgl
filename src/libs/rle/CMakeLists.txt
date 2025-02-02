set(LVGL_LIBS_RLE_SOURCES 
    lv_rle.c
)

# Create the main static library for this directory
add_library(lvgl_libs_rle STATIC ${LVGL_LIBS_RLE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_rle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
