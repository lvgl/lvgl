CSRCS += lv_gpu_swm341_dma2d.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d"
