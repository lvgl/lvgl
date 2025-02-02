set(LVGL_LAYOUT_FLEX_SOURCES 
    lv_flex.c
)

# Create the main static library for this directory
add_library(lvgl_layout_flex STATIC ${LVGL_LAYOUT_FLEX_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_layout_flex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
