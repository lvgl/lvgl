#
# Using the Shadow style properties
#

style = lv.style_t()
style.init()

# Set a background color and a radius
style.set_radius(5)
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 1))

# Add a shadow
style.set_shadow_width(8)
style.set_shadow_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_shadow_ofs_x(10)
style.set_shadow_ofs_y(20)

# Create an object with the new style
obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)
obj.center()
