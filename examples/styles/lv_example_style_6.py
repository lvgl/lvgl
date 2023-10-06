# Create an image from the png file
try:
    with open('../assets/img_cogwheel_argb.png', 'rb') as f:
        png_data = f.read()
except:
    print("Could not find img_cogwheel_argb.png")
    sys.exit()

image_cogwheel_argb = lv.image_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

#
# Using the Image style properties
#
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(5)
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 3))
style.set_border_width(2)
style.set_border_color(lv.palette_main(lv.PALETTE.BLUE))

style.set_image_recolor(lv.palette_main(lv.PALETTE.BLUE))
style.set_image_recolor_opa(lv.OPA._50)
style.set_transform_rotation(300)

# Create an object with the new style
obj = lv.image(lv.scr_act())
obj.add_style(style, 0)

obj.set_src(image_cogwheel_argb)

obj.center()
