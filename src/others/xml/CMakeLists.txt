set(SOURCE_FILES lv_xml_parser.c
	lv_xml_widget.c
	lv_xml.c
	lv_xml_base_types.c
	lv_xml_component.c
	lv_xml_style.c
	lv_xml_utils.c)

# Create the object library for this directory
add_library(lvgl_others_xml OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_xml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(parsers)
target_link_libraries(lvgl_others_xml PRIVATE lvgl_others_xml_parsers)
