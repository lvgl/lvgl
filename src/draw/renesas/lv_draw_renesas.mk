CSRCS += lv_gpu_d2_ra6m3.c
CSRCS += lv_gpu_d2_draw_label.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/renesas
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/renesas

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/renesas"
