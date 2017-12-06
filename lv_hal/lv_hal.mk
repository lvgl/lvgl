CSRCS += lv_hal_disp.c
CSRCS += lv_hal_indev.c
CSRCS += lv_hal_tick.c

DEPPATH += --dep-path lvgl/lv_hal
VPATH += :lvgl/lv_hal

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_hal"
