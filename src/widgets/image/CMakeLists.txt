set(SOURCE_FILES lv_image.c)
# Create the main static library for this directory
add_library(lvgl_widget_image STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})