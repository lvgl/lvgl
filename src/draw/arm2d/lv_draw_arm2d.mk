CSRCS += lv_gpu_arm2d.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d"
