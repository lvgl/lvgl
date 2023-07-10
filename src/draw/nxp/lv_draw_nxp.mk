DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp"

include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/pxp/lv_draw_nxp_pxp.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite/lv_draw_nxp_vglite.mk
