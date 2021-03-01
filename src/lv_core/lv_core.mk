CSRCS += lv_disp.c
CSRCS += lv_group.c
CSRCS += lv_indev.c
CSRCS += lv_indev_scroll.c
CSRCS += lv_obj.c
CSRCS += lv_obj_class.c
CSRCS += lv_obj_draw.c
CSRCS += lv_obj_pos.c
CSRCS += lv_obj_scroll.c
CSRCS += lv_obj_style.c
CSRCS += lv_obj_tree.c
CSRCS += lv_refr.c
CSRCS += lv_theme.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_core"
