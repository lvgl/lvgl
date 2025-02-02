set(SOURCE_FILES lv_tileview.c)
# Create the main static library for this directory
add_library(lvgl_widget_tileview STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_tileview PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})