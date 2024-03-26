LVGL_PATH := $(CURDIR)/../..
ARCH = x86_64
CC = clang
$(info LVGL_PATH is set to: $(LVGL_PATH))

CSRCS  = $(shell find $(LVGL_PATH)/src/core -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/display -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/draw -maxdepth 1 -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/draw/sw -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/font -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/indev -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/layouts -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/libs/bin_decoder -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/misc -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/osal -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/stdlib -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/themes -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/tick -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src/widgets -type f -name '*.c')
CSRCS += $(shell find $(LVGL_PATH)/src -maxdepth 1 -type f -name '*.c')

CSRCS += test.c
CFLAGS = -Werror  -I$(LVGL_PATH)/.. -I$(CURDIR) \
	-target $(ARCH)-unknown-windows -ffreestanding -fshort-wchar  -mno-red-zone  -fno-stack-protector -fno-stack-check -mno-red-zone \
	-DLV_CONF_SKIP=1 \
	-DLV_BUILD_TEST=1 \
	-DLV_COLOR_DEPTH=32 \
	-DLV_USE_STDLIB_MALLOC=LV_STDLIB_BUILTIN \
	-DLV_USE_STDLIB_STRING=LV_STDLIB_BUILTIN \
	-DLV_USE_STDLIB_SPRINTF=LV_STDLIB_BUILTIN \
	-DLV_STDINT_INCLUDE=\"efi.h\" \
	-DLV_STDINT_INCLUDE=\"efi.h\" \
	-DLV_STDDEF_INCLUDE=\"efi.h\" \
	-DLV_STDBOOL_INCLUDE=\"efi.h\" \
	-DLV_INTTYPES_INCLUDE=\"efi.h\" \
	-DLV_LIMITS_INCLUDE=\"efi.h\" \
	-DLV_STDARG_INCLUDE=\"efi.h\"

LDFLAGS = -target $(ARCH)-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link

COBJS := $(patsubst $(LVGL_PATH)%,objs/%, $(CSRCS:.c=.o))

test_file: $(COBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

objs/%.o: $(LVGL_PATH)%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -r $(CURDIR)/objs
	rm test_file