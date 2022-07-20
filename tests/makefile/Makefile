LVGL_PATH := $(CURDIR)/../..
include ../../lvgl.mk

CSRCS += test.c
CFLAGS += -DLV_CONF_SKIP=1 -I$(LVGL_PATH)/..
COBJS := $(patsubst %.c, %.o, $(CSRCS))

test_file: $(COBJS)
	$(CC) -o $@ $^
