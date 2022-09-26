#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
import fs_driver
from img_bulb_gif import img_bulb_gif_map

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')
#
# Open a GIF image from a file and a variable
#
img_bulb_gif = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 0, "h": 0,  "cf": lv.img.CF.RAW},
        "data_size": 0,
        "data": img_bulb_gif_map,
    }
)
img1 = lv.gif(lv.scr_act())
img1.set_src(img_bulb_gif)
img1.align(lv.ALIGN.RIGHT_MID, -150, 0)

img2 = lv.gif(lv.scr_act())
# The File system is attached to letter 'S'

img2.set_src("S:bulb.gif")
img2.align(lv.ALIGN.RIGHT_MID, -250, 0)
