set(LVGL_LIBS_BIN_DECODER_SOURCES 
    lv_bin_decoder.c
)

# Create the main static library for this directory
add_library(lvgl_libs_bin_decoder STATIC ${LVGL_LIBS_BIN_DECODER_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_bin_decoder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
