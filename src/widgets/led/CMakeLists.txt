set(SOURCE_FILES lv_led.c)
# Create the main static library for this directory
add_library(lvgl_widget_led STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_led PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})