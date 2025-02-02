set(SOURCE_FILES lv_spinbox.c)
# Create the object library for this directory
add_library(lvgl_widget_spinbox OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_spinbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
