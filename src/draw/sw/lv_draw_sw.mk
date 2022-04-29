CSRCS += lv_draw_sw.c
CSRCS += lv_draw_sw_arc.c
CSRCS += lv_draw_sw_blend.c
CSRCS += lv_draw_sw_dither.c
CSRCS += lv_draw_sw_gradient.c
CSRCS += lv_draw_sw_img.c
CSRCS += lv_draw_sw_letter.c
CSRCS += lv_draw_sw_line.c
CSRCS += lv_draw_sw_polygon.c
CSRCS += lv_draw_sw_rect.c
CSRCS += lv_draw_sw_transform.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw"
