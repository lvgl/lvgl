set(LVGL_LIBS_BARCODE_SOURCES 
    code128.c
    lv_barcode.c
)

# Create the main static library for this directory
add_library(lvgl_libs_barcode STATIC ${LVGL_LIBS_BARCODE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_barcode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
