set(SOURCE_FILES lv_label.c)
# Create the main static library for this directory
add_library(lvgl_widget_label OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_label PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
