#
# Using the border style properties
#
style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(10)
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 1))

# Add border to the bottom+right
style.set_border_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_border_width(5)
style.set_border_opa(lv.OPA._50)
style.set_border_side(lv.BORDER_SIDE.BOTTOM | lv.BORDER_SIDE.RIGHT)

# Create an object with the new style
obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)
obj.center()
