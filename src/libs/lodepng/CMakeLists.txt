set(LVGL_LIBS_LODEPNG_SOURCES 
    lv_lodepng.c
    lodepng.c
)

# Create the main static library for this directory
add_library(lvgl_libs_lodepng STATIC ${LVGL_LIBS_LODEPNG_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_lodepng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
