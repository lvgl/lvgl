
def ofs_y_anim(image, v):
    image.set_offset_y(v)
    # print(image,v)

# Create an image from the png file
try:
    with open('../../assets/img_skew_strip.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find img_skew_strip.png")
    sys.exit()

image_skew_strip = lv.image_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

#
# Image styling and offset
#

style = lv.style_t()
style.init()
style.set_bg_color(lv.palette_main(lv.PALETTE.YELLOW))
style.set_bg_opa(lv.OPA.COVER)
style.set_image_recolor_opa(lv.OPA.COVER)
style.set_image_recolor(lv.color_black())

image = lv.image(lv.scr_act())
image.add_style(style, 0)
image.set_src(image_skew_strip)
image.set_size(150, 100)
image.center()

a = lv.anim_t()
a.init()
a.set_var(image)
a.set_values(0, 100)
a.set_time(3000)
a.set_playback_time(500)
a.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
a.set_custom_exec_cb(lambda a,val: ofs_y_anim(image,val))
lv.anim_t.start(a)

