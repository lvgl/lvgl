CSRCS += lv_gpu_nxp_pxp.c
CSRCS += lv_gpu_nxp_pxp_osa.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_pxp
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_pxp

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_pxp"
