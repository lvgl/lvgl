CSRCS += lv_draw_basic.c
CSRCS += lv_draw.c
CSRCS += lv_draw_rect.c
CSRCS += lv_draw_label.c
CSRCS += lv_draw_line.c
CSRCS += lv_draw_img.c
CSRCS += lv_draw_arc.c
CSRCS += lv_draw_triangle.c
CSRCS += lv_img_decoder.c
CSRCS += lv_img_cache.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_draw
VPATH += :$(LVGL_DIR)/lvgl/src/lv_draw

CFLAGS += "-I$(LVGL_DIR)lvgl/src/lv_draw"
