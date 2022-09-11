LVGL_PATH ?= ${shell pwd}/lvgl

CSRCS += $(shell find $(LVGL_PATH)/src -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/demos -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/examples -type f -name '*.c')
CFLAGS += "-I$(LVGL_PATH)"
