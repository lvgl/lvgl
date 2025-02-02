set(LVGL_LIBS_QRCODE_SOURCES 
    lv_qrcode.c
    qrcodegen.c
)

# Create the main static library for this directory
add_library(lvgl_libs_qrcode STATIC ${LVGL_LIBS_QRCODE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_qrcode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
