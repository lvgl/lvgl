CSRCS += lv_draw_arc_gpu_sdl.c
CSRCS += lv_draw_blend_gpu_sdl.c
CSRCS += lv_draw_img_gpu_sdl.c
CSRCS += lv_draw_label_gpu_sdl.c
CSRCS += lv_draw_line_gpu_sdl.c
CSRCS += lv_draw_rect_gpu_sdl.c
CSRCS += lv_gpu_draw_cache.c
CSRCS += lv_gpu_sdl_lru.c
CSRCS += lv_gpu_sdl_mask.c
CSRCS += lv_gpu_sdl_utils.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl"
