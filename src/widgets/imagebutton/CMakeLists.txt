set(SOURCE_FILES lv_imagebutton.c)
# Create the main static library for this directory
add_library(lvgl_widget_imagebutton STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_imagebutton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})