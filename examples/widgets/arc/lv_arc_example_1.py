# Create style for the Arcs
style = lv.style_t()
lv.style_copy(style, lv.style_plain)
style.line.color = lv.color_make(0,0,255) # Arc color
style.line.width = 8                      # Arc width

# Create an Arc
arc = lv.arc(lv.scr_act())
arc.set_style(lv.arc.STYLE.MAIN, style)   # Use the new style
arc.set_angles(90, 60)
arc.set_size(150, 150)
arc.align(None, lv.ALIGN.CENTER, 0, 0)