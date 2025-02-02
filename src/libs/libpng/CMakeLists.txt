set(LVGL_LIBS_LIBPNG_SOURCES 
    lv_libpng.c
)

# Create the main static library for this directory
add_library(lvgl_libs_libpng STATIC ${LVGL_LIBS_LIBPNG_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_libpng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
