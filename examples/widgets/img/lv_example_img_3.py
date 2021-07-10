#!/opt/bin/lv_micropython -i
import usys as sys
import lvgl as lv
import display_driver
from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

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

def set_angle(img, v):
    img.set_angle(v)

def set_zoom(img, v):
    img.set_zoom(v)


#
# Show transformations (zoom and rotation) using a pivot point.
#

# Now create the actual image
img = lv.img(lv.scr_act())
img.set_src(img_cogwheel_argb)
img.align(lv.ALIGN.CENTER, 50, 50)
img.set_pivot(0, 0)               # Rotate around the top left corner

a1 = lv.anim_t()
a1.init()
a1.set_var(img)
a1.set_custom_exec_cb(lambda a,val: set_angle(img,val))
a1.set_values(0, 3600)
a1.set_time(5000)
a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
lv.anim_t.start(a1)

a2 = lv.anim_t()
a2.init()
a2.set_var(img)
a2.set_custom_exec_cb(lambda a,val: set_zoom(img,val))
a2.set_values(128, 256)
a2.set_time(5000)
a2.set_playback_time(3000)
a2.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
lv.anim_t.start(a2)


