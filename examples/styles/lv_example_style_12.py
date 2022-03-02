#
# Local styles
#

style = lv.style_t()
style.init()
style.set_bg_color(lv.palette_main(lv.PALETTE.GREEN))
style.set_border_color(lv.palette_lighten(lv.PALETTE.GREEN, 3))
style.set_border_width(3)

obj = lv.obj(lv.scr_act())
obj.add_style(style, 0)

# Overwrite the background color locally
obj.set_style_bg_color(lv.palette_main(lv.PALETTE.ORANGE), lv.PART.MAIN)

obj.center()
