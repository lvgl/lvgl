#
# Using the outline style properties
#

style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(5)
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 1))

# Add outline
style.set_outline_width(2)
style.set_outline_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_outline_pad(8)

# Create an object with the new style
obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)
obj.center()
