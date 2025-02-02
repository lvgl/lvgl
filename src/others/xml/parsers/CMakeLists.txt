set(SOURCE_FILES lv_xml_button_parser.c
	lv_xml_label_parser.c
	lv_xml_slider_parser.c
	lv_xml_chart_parser.c
	lv_xml_image_parser.c
	lv_xml_obj_parser.c
	lv_xml_tabview_parser.c)
# Create the main static library for this directory
add_library(lvgl_others_xml_parsers STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_xml_parsers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
