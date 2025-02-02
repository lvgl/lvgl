set(SOURCE_FILES lv_dropdown_properties.c
lv_image_properties.c
lv_keyboard_properties.c
lv_label_properties.c
lv_roller_properties.c
lv_textarea_properties.c
lv_style_properties.c
lv_animimage_properties.c
lv_slider_properties.c
lv_obj_properties.c)
# Create the main static library for this directory
add_library(lvgl_widget_property STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_property PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})