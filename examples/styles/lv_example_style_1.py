#
# Using the Size, Position and Padding style properties
#
style = lv.style_t()
style.init()
style.set_radius(5)

# Make a gradient
style.set_width(150)
style.set_height(lv.SIZE.CONTENT)

style.set_pad_ver(20)
style.set_pad_left(5)

style.set_x(lv.pct(50))
style.set_y(80)

# Create an object with the new style
obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)

label = lv.label(obj)
label.set_text("Hello")

