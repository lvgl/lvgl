CSRCS += lv_draw_vglite_arc.c
CSRCS += lv_draw_vglite_blend.c
CSRCS += lv_draw_vglite_rect.c
CSRCS += lv_gpu_nxp_vglite.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite"
