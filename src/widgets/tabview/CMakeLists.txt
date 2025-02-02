set(SOURCE_FILES lv_tabview.c)
# Create the main static library for this directory
add_library(lvgl_widget_tabview STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_tabview PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})