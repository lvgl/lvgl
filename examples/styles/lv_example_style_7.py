#
# Using the Arc style properties
#
style = lv.style_t()
style.init()

style.set_arc_color(lv.palette_main(lv.PALETTE.RED))
style.set_arc_width(4)

# Create an object with the new style
obj = lv.arc(lv.screen_active())
obj.add_style(style, 0)
obj.center()
