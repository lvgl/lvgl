CSRCS += lv_circ.c
CSRCS += lv_area.c
CSRCS += lv_task.c
CSRCS += lv_fs.c
CSRCS += lv_anim.c
CSRCS += lv_mem.c
CSRCS += lv_ll.c
CSRCS += lv_color.c
CSRCS += lv_txt.c
CSRCS += lv_math.c
CSRCS += lv_log.c
CSRCS += lv_gc.c
CSRCS += lv_utils.c
CSRCS += lv_async.c
CSRCS += lv_printf.c
CSRCS += lv_bidi.c


DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_misc
VPATH += :$(LVGL_DIR)/lvgl/src/lv_misc

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_misc"
