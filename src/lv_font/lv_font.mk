CSRCS += lv_font.c
CSRCS += lv_font_fmt_txt.c
CSRCS += lv_font_montserrat_12.c
CSRCS += lv_font_montserrat_14.c
CSRCS += lv_font_montserrat_16.c
CSRCS += lv_font_montserrat_18.c
CSRCS += lv_font_montserrat_20.c
CSRCS += lv_font_montserrat_22.c
CSRCS += lv_font_montserrat_14.c
CSRCS += lv_font_montserrat_26.c
CSRCS += lv_font_montserrat_28.c
CSRCS += lv_font_montserrat_30.c
CSRCS += lv_font_montserrat_32.c
CSRCS += lv_font_montserrat_12_subpx.c
CSRCS += lv_font_montserrat_28_compressed.c
CSRCS += lv_font_unscii_8.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_font
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_font

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_font"
