obj1 = lv.obj(lv.scr_act())
obj1.set_size(100, 50)
obj1.set_style(lv.style_plain_color)
obj1.align(None, lv.ALIGN.CENTER, -60, -30)

# Copy the previous object and enable drag
obj2 = lv.obj(lv.scr_act(), obj1)
obj2.set_style(lv.style_pretty_color)
obj2.align(None, lv.ALIGN.CENTER, 0, 0)
obj2.set_drag(True)

style_shadow = lv.style_t()
lv.style_copy(style_shadow, lv.style_pretty)
style_shadow.body.shadow.width = 6
style_shadow.body.radius = 800 # large enough to make it round

# Copy the previous object (drag is already enabled)
obj3 = lv.obj(lv.scr_act(), obj2)
obj3.set_style(style_shadow)
obj3.align(None, lv.ALIGN.CENTER, 60, 30)