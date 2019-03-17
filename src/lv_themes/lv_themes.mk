CSRCS += lv_theme_alien.c
CSRCS += lv_theme.c
CSRCS += lv_theme_default.c
CSRCS += lv_theme_night.c
CSRCS += lv_theme_templ.c
CSRCS += lv_theme_zen.c
CSRCS += lv_theme_material.c
CSRCS += lv_theme_nemo.c
CSRCS += lv_theme_mono.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_themes
VPATH += :$(LVGL_DIR)/lvgl/src/lv_themes

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_themes"
