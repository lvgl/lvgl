#!/opt/bin/lv_micropython -i
import usys as sys
import lvgl as lv
import display_driver

# Create an image from the png file
try:
    with open('../../assets/img_cogwheel_argb.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find img_cogwheel_argb.png")
    sys.exit()

img_cogwheel_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

img1 = lv.img(lv.scr_act())
img1.set_src(img_cogwheel_argb)
img1.align(lv.ALIGN.CENTER, 0, -20)
img1.set_size(200, 200)

img2 = lv.img(lv.scr_act())
img2.set_src(lv.SYMBOL.OK + "Accept")
img2.align_to(img1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)
