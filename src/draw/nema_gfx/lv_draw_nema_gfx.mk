CSRCS += lv_draw_nema_gfx.c
CSRCS += lv_draw_nema_gfx_letter.c
CSRCS += lv_draw_nema_gfx_img.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nema_gfx
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nema_gfx

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/nema_gfx"
