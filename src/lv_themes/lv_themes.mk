CSRCS += lv_theme.c

CSRCS += lv_theme_empty.c
CSRCS += lv_theme_material.c
CSRCS += lv_theme_mono.c
CSRCS += lv_theme_template.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_themes
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_themes

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_themes"
