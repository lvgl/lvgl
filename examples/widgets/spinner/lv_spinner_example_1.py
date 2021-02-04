# Create a style for the Preloader
style = lv.style_t()
lv.style_copy(style, lv.style_plain)
style.line.width = 10                          # 10 px thick arc
style.line.color = lv.color_hex3(0x258)        # Blueish arc color

style.body.border.color = lv.color_hex3(0xBBB) # Gray background color
style.body.border.width = 10
style.body.padding.left = 0

# Create a Preloader object
preload = lv.preload(lv.scr_act())
preload.set_size(100, 100)
preload.align(None, lv.ALIGN.CENTER, 0, 0)
preload.set_style(lv.preload.STYLE.MAIN, style)