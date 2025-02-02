set(SOURCE_FILES
	lv_demo_flex_layout_ctrl_pad.c
	lv_demo_flex_layout_main.c
	lv_demo_flex_layout_view_child_node.c
	lv_demo_flex_layout_view_ctrl_pad.c
	lv_demo_flex_layout_flex_loader.c
	lv_demo_flex_layout_view.c)

# Create the object library for this directory
add_library(lvgl_demo_flex_layout STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_flex_layout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
