CSRCS += lv_gpu_nxp_vglite.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_vglite
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_vglite

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp_vglite"
