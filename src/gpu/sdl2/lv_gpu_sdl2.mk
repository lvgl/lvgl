CSRCS += lv_draw_arc_gpu_sdl2.c
CSRCS += lv_draw_rect_gpu_sdl2.c
CSRCS += lv_gpu_draw_cache.c
CSRCS += lv_gpu_sdl2_utils.c
CSRCS += lv_gpu_sdl2_lru.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl2"
