CSRCS += lv_font_builtin.c
CSRCS += lv_font_dejavu_10.c
CSRCS += lv_font_dejavu_20.c
CSRCS += lv_font_dejavu_30.c
CSRCS += lv_font_dejavu_40.c
CSRCS += lv_font_dejavu_10_cyrillic.c
CSRCS += lv_font_dejavu_20_cyrillic.c
CSRCS += lv_font_dejavu_30_cyrillic.c
CSRCS += lv_font_dejavu_40_cyrillic.c
CSRCS += lv_font_dejavu_10_latin_sup.c
CSRCS += lv_font_dejavu_20_latin_sup.c
CSRCS += lv_font_dejavu_30_latin_sup.c
CSRCS += lv_font_dejavu_40_latin_sup.c
CSRCS += lv_font_symbol_10.c
CSRCS += lv_font_symbol_20.c
CSRCS += lv_font_symbol_30.c
CSRCS += lv_font_symbol_40.c
CSRCS += lv_font_monospace_8.c

DEPPATH += --dep-path lvgl/lv_fonts
VPATH += :lvgl/lv_fonts

CFLAGS += "-I$(LVGL_DIR)/lvgl/lv_fonts"
