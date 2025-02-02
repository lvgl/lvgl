set(LVGL_DISPLAY_SOURCES
    lv_display.c
)

# Create the object library for this directory
add_library(lvgl_display OBJECT ${LVGL_DISPLAY_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_display PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
