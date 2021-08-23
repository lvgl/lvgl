CSRCS += lv_draw_arc_gpu_sdl2.c
CSRCS += lv_draw_blend_gpu_sdl2.c
CSRCS += lv_draw_img_gpu_sdl2.c
CSRCS += lv_draw_label_gpu_sdl2.c
CSRCS += lv_draw_line_gpu_sdl2.c
CSRCS += lv_draw_rect_gpu_sdl2.c
CSRCS += lv_gpu_draw_cache.c
CSRCS += lv_gpu_sdl2_lru.c
CSRCS += lv_gpu_sdl2_mask.c
CSRCS += lv_gpu_sdl2_utils.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2"
