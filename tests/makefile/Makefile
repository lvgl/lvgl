LVGL_PATH := $(CURDIR)/../..
$(info LVGL_PATH is set to: $(LVGL_PATH))
include ../../lvgl.mk

CSRCS += test.c
CFLAGS += -Werror -DLV_CONF_SKIP=1 -DLV_BUILD_TEST=1 -I$(LVGL_PATH)/..
COBJS := $(patsubst %.c, %.o, $(CSRCS))

test_file: $(COBJS)
	$(CC) -o $@ $^
