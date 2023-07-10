CSRCS += lv_draw_vglite.c
CSRCS += lv_draw_vglite_arc.c
CSRCS += lv_draw_vglite_blend.c
CSRCS += lv_draw_vglite_line.c
CSRCS += lv_draw_vglite_rect.c
CSRCS += lv_vglite_buf.c
CSRCS += lv_vglite_utils.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite"
