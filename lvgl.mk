CSRCS += $(shell find $(LVGL_DIR_NAME)/src -type f -name '*.c')
CSRCS += $(shell find $(LVGL_DIR_NAME)/demos -type f -name '*.c')
CSRCS += $(shell find $(LVGL_DIR_NAME)/examples -type f -name '*.c')
CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)"
