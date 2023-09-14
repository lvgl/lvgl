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

def set_angle(image, v):
    image.set_angle(v)

def set_zoom(image, v):
    image.set_zoom(v)


#
# Show transformations (zoom and rotation) using a pivot point.
#

# Now create the actual image
image = lv.image(lv.scr_act())
image.set_src(image_cogwheel_argb)
image.align(lv.ALIGN.CENTER, 50, 50)
image.set_pivot(0, 0)               # Rotate around the top left corner

a1 = lv.anim_t()
a1.init()
a1.set_var(image)
a1.set_custom_exec_cb(lambda a,val: set_angle(image,val))
a1.set_values(0, 3600)
a1.set_time(5000)
a1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
lv.anim_t.start(a1)

a2 = lv.anim_t()
a2.init()
a2.set_var(image)
a2.set_custom_exec_cb(lambda a,val: set_zoom(image,val))
a2.set_values(128, 256)
a2.set_time(5000)
a2.set_playback_time(3000)
a2.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
lv.anim_t.start(a2)


