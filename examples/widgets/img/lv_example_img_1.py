#!/opt/bin/lv_micropython -i
import usys as sys
import lvgl as lv
import display_driver

# Create an image from the png file
try:
    with open('../../assets/image_cogwheel_argb.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find image_cogwheel_argb.png")
    sys.exit()

image_cogwheel_argb = lv.image_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

image1 = lv.image(lv.scr_act())
image1.set_src(image_cogwheel_argb)
image1.align(lv.ALIGN.CENTER, 0, -20)
image1.set_size(200, 200)

image2 = lv.image(lv.scr_act())
image2.set_src(lv.SYMBOL.OK + "Accept")
image2.align_to(image1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)
