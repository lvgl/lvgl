set(SOURCE_FILES 
	lv_vglite_buf.c
	lv_vglite_matrix.c
	lv_vglite_path.c
	lv_draw_buf_vglite.c
	lv_draw_vglite.c
	lv_draw_vglite_arc.c
	lv_draw_vglite_border.c
	lv_draw_vglite_fill.c
	lv_draw_vglite_img.c
	lv_draw_vglite_label.c
	lv_draw_vglite_layer.c
	lv_draw_vglite_line.c
	lv_draw_vglite_triangle.c
	lv_vglite_utils.c)
# Create the main static library for this directory
add_library(lvgl_draw_nxp_vglite STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_nxp_vglite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
