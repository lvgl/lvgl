CSRCS += lv_draw_sdl.c
CSRCS += lv_draw_sdl_arc.c
CSRCS += lv_draw_sdl_bg.c
CSRCS += lv_draw_sdl_composite.c
CSRCS += lv_draw_sdl_img.c
CSRCS += lv_draw_sdl_label.c
CSRCS += lv_draw_sdl_line.c
CSRCS += lv_draw_sdl_mask.c
CSRCS += lv_draw_sdl_polygon.c
CSRCS += lv_draw_sdl_rect.c
CSRCS += lv_draw_sdl_stack_blur.c
CSRCS += lv_draw_sdl_texture_cache.c
CSRCS += lv_draw_sdl_utils.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sdl
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sdl

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sdl"
