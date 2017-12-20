CSRCS += lv_draw_vbasic.c
CSRCS += lv_draw.c
CSRCS += lv_draw_rbasic.c

DEPPATH += --dep-path lvgl/lv_draw
VPATH += :lvgl/lv_draw

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_draw"
