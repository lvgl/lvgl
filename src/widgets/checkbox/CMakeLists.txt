set(SOURCE_FILES lv_checkbox.c)
# Create the main static library for this directory
add_library(lvgl_widget_checkbox STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_checkbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})