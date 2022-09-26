#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
import fs_driver

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

wp = lv.img(lv.scr_act())
# The File system is attached to letter 'S'

wp.set_src("S:small_image.sjpg")
wp.center()
