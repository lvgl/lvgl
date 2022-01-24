#
# Using multiple styles
#
# A base style

style_base =  lv.style_t()
style_base.init()
style_base.set_bg_color(lv.palette_main(lv.PALETTE.LIGHT_BLUE))
style_base.set_border_color(lv.palette_darken(lv.PALETTE.LIGHT_BLUE, 3))
style_base.set_border_width(2)
style_base.set_radius(10)
style_base.set_shadow_width(10)
style_base.set_shadow_ofs_y(5)
style_base.set_shadow_opa(lv.OPA._50)
style_base.set_text_color(lv.color_white())
style_base.set_width(100)
style_base.set_height(lv.SIZE.CONTENT)

# Set only the properties that should be different
style_warning = lv.style_t()
style_warning.init()
style_warning.set_bg_color(lv.palette_main(lv.PALETTE.YELLOW))
style_warning.set_border_color(lv.palette_darken(lv.PALETTE.YELLOW, 3))
style_warning.set_text_color(lv.palette_darken(lv.PALETTE.YELLOW, 4))

# Create an object with the base style only
obj_base = lv.obj(lv.scr_act())
obj_base.add_style(style_base, 0)
obj_base.align(lv.ALIGN.LEFT_MID, 20, 0)

label = lv.label(obj_base)
label.set_text("Base")
label.center()

# Create an other object with the base style and earnings style too
obj_warning = lv.obj(lv.scr_act())
obj_warning.add_style(style_base, 0)
obj_warning.add_style(style_warning, 0)
obj_warning.align(lv.ALIGN.RIGHT_MID, -20, 0)

label = lv.label(obj_warning)
label.set_text("Warning")
label.center()

