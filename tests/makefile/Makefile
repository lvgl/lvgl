LVGL_DIR := $(CURDIR)/../..
include ../../lvgl.mk

CSRCS += test.c
CFLAGS += -DLV_CONF_SKIP=1 -DLV_BUILD_TEST=1 -I$(LVGL_DIR)/..
COBJS := $(patsubst %.c, %.o, $(CSRCS))

test_file: $(COBJS)
	$(CC) -o $@ $^
