set(LVGL_LIBS_LZ4_SOURCES 
    lz4.c
)

# Create the main static library for this directory
add_library(lvgl_libs_lz4 STATIC ${LVGL_LIBS_LZ4_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_lodepng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
