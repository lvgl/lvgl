set(SOURCE_FILES lv_draw_nema_gfx_stm32_hal.c
	lv_draw_nema_gfx_utils.c
	lv_nema_gfx_path.c
	lv_draw_nema_gfx.c
	lv_draw_nema_gfx_arc.c
	lv_draw_nema_gfx_border.c
	lv_draw_nema_gfx_fill.c
	lv_draw_nema_gfx_img.c
	lv_draw_nema_gfx_label.c
	lv_draw_nema_gfx_layer.c
	lv_draw_nema_gfx_line.c
	lv_draw_nema_gfx_triangle.c)
# Create the main static library for this directory
add_library(lvgl_draw_nema_gfx STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_nema_gfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
