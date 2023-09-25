#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
import fs_driver

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

image = lv.image(lv.scr_act())
image.set_src("S:png_demo.png")
image.center()
