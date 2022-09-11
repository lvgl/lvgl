#
# Using the background style properties
#
style = lv.style_t()
style.init()
style.set_radius(5)

# Make a gradient
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 1))
style.set_bg_grad_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_bg_grad_dir(lv.GRAD_DIR.VER)

# Shift the gradient to the bottom
style.set_bg_main_stop(128)
style.set_bg_grad_stop(192)

# Create an object with the new style
obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)
obj.center()
