#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
from imagetools import get_png_info, open_png
from img_wink_png import img_wink_png_map
# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

img_wink_png = lv.img_dsc_t(
    {
        "header": {"always_zero": 0, "w": 50, "h": 50,  "cf": lv.img.CF.RAW_ALPHA},
        "data_size": 5158,
        "data": img_wink_png_map,
    }
)
img1 = lv.img(lv.scr_act())
img1.set_src(img_wink_png)
img1.align(lv.ALIGN.RIGHT_MID, -250, 0)

# Create an image from the png file
try:
    with open('wink.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find wink.png")
    sys.exit()

wink_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

img2 = lv.img(lv.scr_act())
img2.set_src(wink_argb)
img2.align(lv.ALIGN.RIGHT_MID, -150, 0)
