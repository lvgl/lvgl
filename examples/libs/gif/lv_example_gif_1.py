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
image_bulb_gif = lv.image_dsc_t(
    {
        "header": {"always_zero": 0, "w": 0, "h": 0,  "cf": lv.COLOR_FORMAT.RAW},
        "data_size": 0,
        "data": img_bulb_gif_map,
    }
)
image1 = lv.gif(lv.scr_act())
image1.set_src(image_bulb_gif)
image1.align(lv.ALIGN.RIGHT_MID, -150, 0)

image2 = lv.gif(lv.scr_act())
# The File system is attached to letter 'S'

image2.set_src("S:bulb.gif")
image2.align(lv.ALIGN.RIGHT_MID, -250, 0)
