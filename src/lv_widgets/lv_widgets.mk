CSRCS += lv_arc.c
CSRCS += lv_bar.c
CSRCS += lv_checkbox.c
CSRCS += lv_dropdown.c
CSRCS += lv_line.c
CSRCS += lv_roller.c
CSRCS += lv_table.c
CSRCS += lv_btn.c
CSRCS += lv_chart.c
CSRCS += lv_canvas.c
CSRCS += lv_gauge.c
CSRCS += lv_label.c
CSRCS += lv_slider.c
CSRCS += lv_textarea.c
CSRCS += lv_btnmatrix.c
CSRCS += lv_img.c
CSRCS += lv_imgbtn.c
CSRCS += lv_linemeter.c
CSRCS += lv_switch.c
CSRCS += lv_objmask.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_widgets
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_widgets

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/lv_widgets"
