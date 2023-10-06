#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
from img_wink_png import img_wink_png_map

image_wink_png = lv.image_dsc_t(
    {
        "header": {"always_zero": 0, "w": 50, "h": 50,  "cf": lv.COLOR_FORMAT.RAW_ALPHA},
        "data_size": 5158,
        "data": img_wink_png_map,
    }
)
image1 = lv.image(lv.scr_act())
image1.set_src(image_wink_png)
image1.align(lv.ALIGN.RIGHT_MID, -250, 0)

# Create an image from the png file
try:
    with open('wink.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find wink.png")
    sys.exit()

wink_argb = lv.image_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

image2 = lv.image(lv.scr_act())
image2.set_src(wink_argb)
image2.align(lv.ALIGN.RIGHT_MID, -150, 0)
