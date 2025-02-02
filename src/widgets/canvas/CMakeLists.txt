set(SOURCE_FILES lv_canvas.c)
# Create the object library for this directory
add_library(lvgl_widget_canvas OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
