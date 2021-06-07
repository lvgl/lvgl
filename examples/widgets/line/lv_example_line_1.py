# Create an array for the points of the line
line_points = [ {"x":5, "y":5}, 
                {"x":70, "y":70}, 
                {"x":120, "y":10}, 
                {"x":180, "y":60}, 
                {"x":240, "y":10}]

# Create style
style_line = lv.style_t()
style_line.init()
style_line.set_line_width(8)
style_line.set_line_color(lv.palette_main(lv.PALETTE.BLUE))
style_line.set_line_rounded(True)

# Create a line and apply the new style
line1 = lv.line(lv.scr_act())
line1.set_points(line_points, 5)     # Set the points
line1.add_style(style_line, 0)
line1.center()

