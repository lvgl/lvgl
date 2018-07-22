CSRCS += lv_font.c
CSRCS += lv_circ.c
CSRCS += lv_area.c
CSRCS += lv_task.c
CSRCS += lv_fs.c
CSRCS += lv_anim.c
CSRCS += lv_mem.c
CSRCS += lv_ll.c
CSRCS += lv_color.c
CSRCS += lv_txt.c
CSRCS += lv_ufs.c
CSRCS += lv_trigo.c
CSRCS += lv_math.c

DEPPATH += --dep-path lvgl/lv_misc
VPATH += :lvgl/lv_misc

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_misc"
