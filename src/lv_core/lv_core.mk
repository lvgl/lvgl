CSRCS += lv_group.c
CSRCS += lv_indev.c
CSRCS += lv_disp.c
CSRCS += lv_obj.c
CSRCS += lv_refr.c
CSRCS += lv_style.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core"
