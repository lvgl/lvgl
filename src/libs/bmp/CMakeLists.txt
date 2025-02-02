set(LVGL_LIBS_BMP_SOURCES 
    lv_bmp.c
)

# Create the object library for this directory
add_library(lvgl_libs_bmp OBJECT ${LVGL_LIBS_BMP_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_bmp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
