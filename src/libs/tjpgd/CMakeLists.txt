set(LVGL_LIBS_TJPGD_SOURCES
    lv_tjpgd.c
)

# Create the main static library for this directory
add_library(lvgl_libs_tjpgd STATIC ${LVGL_LIBS_TJPGD_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_tjpgd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
