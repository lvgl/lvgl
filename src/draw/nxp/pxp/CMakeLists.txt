set(SOURCE_FILES 
	lv_draw_buf_pxp.c
	lv_pxp_cfg.c
	lv_pxp_osa.c
	lv_pxp_utils.c
	lv_draw_pxp.c
	lv_draw_pxp_fill.c
	lv_draw_pxp_img.c
	lv_draw_pxp_layer.c)


if(CONFIG_LV_USE_DRAW_PXP)
	list(APPEND SOURCE_FILES lv_draw_buf_pxp.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_fill.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_img.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_layer.c)
endif()

if(CONFIG_LV_USE_DRAW_PXP OR CONFIG_LV_USE_ROTATE_PXP)
	list(APPEND SOURCE_FILES lv_draw_pxp.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_rotate.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_osa.c)
	list(APPEND SOURCE_FILES lv_draw_pxp_utils.c)
endif()

# Create the main static library for this directory
add_library(lvgl_draw_nxp_pxp STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_nxp_pxp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
