set(SOURCE_FILES lv_bar.c)
# Create the main static library for this directory
add_library(lvgl_widget_bar STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_bar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})