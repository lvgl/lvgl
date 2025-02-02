set(SOURCE_FILES lv_draw_dma2d.c
	lv_draw_dma2d_fill.c
	lv_draw_dma2d_img.c)
# Create the main static library for this directory
add_library(lvgl_draw_dma2d STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_dma2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
