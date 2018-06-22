CSRCS += lv_sys_posix_generic.c

DEPPATH += --dep-path lvgl/lv_sys/posix
VPATH += :lvgl/lv_sys/posix

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_sys/posix"
