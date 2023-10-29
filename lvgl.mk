LVGL_PATH ?= ${shell pwd}/lvgl

CSRCS += $(shell find $(LVGL_PATH)/src -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/demos -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/examples -type f -name '*.c')
CFLAGS += "-I$(LVGL_PATH)"

ifeq ($(CONFIG_LV_USE_THORVG_INTERNAL),y)
CXXSRCS += $(shell find $(LVGL_PATH)/src/libs -type f -name '*.cpp')
endif
