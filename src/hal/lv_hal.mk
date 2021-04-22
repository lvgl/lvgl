CSRCS += lv_hal_disp.c
CSRCS += lv_hal_indev.c
CSRCS += lv_hal_tick.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal"
