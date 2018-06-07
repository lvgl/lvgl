CSRCS += lv_draw_vbasic.c
CSRCS += lv_draw_rbasic.c
CSRCS += lv_draw.c
CSRCS += lv_draw_rect.c
CSRCS += lv_draw_label.c
CSRCS += lv_draw_line.c
CSRCS += lv_draw_img.c
CSRCS += lv_draw_arc.c
CSRCS += lv_draw_triangle.c

DEPPATH += --dep-path lvgl/lv_draw
VPATH += :lvgl/lv_draw

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_draw"
