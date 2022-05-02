CSRCS += lv_draw_pxp_blend.c
CSRCS += lv_gpu_nxp_pxp_osa.c
CSRCS += lv_gpu_nxp_pxp.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/pxp
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/pxp

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/pxp"
