CSRCS += lv_gpu_sdl_draw_arc.c
CSRCS += lv_gpu_sdl_draw_blend.c
CSRCS += lv_gpu_sdl_draw_img.c
CSRCS += lv_gpu_sdl_draw_label.c
CSRCS += lv_gpu_sdl_draw_line.c
CSRCS += lv_gpu_sdl_draw_rect.c
CSRCS += lv_gpu_sdl_lru.c
CSRCS += lv_gpu_sdl_mask.c
CSRCS += lv_gpu_sdl_stack_blur.c
CSRCS += lv_gpu_sdl_texture_cache.c
CSRCS += lv_gpu_sdl_utils.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl"
