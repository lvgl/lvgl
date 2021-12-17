#
# Using the line style properties
#

style = lv.style_t()
style.init()

style.set_line_color(lv.palette_main(lv.PALETTE.GREY))
style.set_line_width(6)
style.set_line_rounded(True)

# Create an object with the new style
obj = lv.line(lv.scr_act())
obj.add_style(style, 0)
p =  [ {"x":10, "y":30}, 
       {"x":30, "y":50}, 
       {"x":100, "y":0}]

obj.set_points(p, 3)

obj.center()
