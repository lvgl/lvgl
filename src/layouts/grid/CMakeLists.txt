set(LVGL_LAYOUT_GRID_SOURCES 
    lv_grid.c
)

# Create the main static library for this directory
add_library(lvgl_layout_grid STATIC ${LVGL_LAYOUT_GRID_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_layout_grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

