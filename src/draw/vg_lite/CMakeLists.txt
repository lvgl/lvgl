set(SOURCE_FILES lv_vg_lite_pending.c
	lv_draw_buf_vg_lite.c
	lv_vg_lite_decoder.c
	lv_vg_lite_grad.c
	lv_vg_lite_path.c
	lv_vg_lite_stroke.c
	lv_vg_lite_utils.c
	lv_draw_vg_lite.c
	lv_draw_vg_lite_arc.c
	lv_draw_vg_lite_border.c
	lv_draw_vg_lite_box_shadow.c
	lv_draw_vg_lite_fill.c
	lv_draw_vg_lite_img.c
	lv_draw_vg_lite_label.c
	lv_draw_vg_lite_layer.c
	lv_draw_vg_lite_line.c
	lv_draw_vg_lite_mask_rect.c
	lv_draw_vg_lite_triangle.c
	lv_draw_vg_lite_vector.c
	lv_vg_lite_math.c)
# Create the main static library for this directory
add_library(lvgl_draw_vg_lite STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_vg_lite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
