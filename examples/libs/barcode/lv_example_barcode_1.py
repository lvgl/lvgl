#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver

bg_color = lv.palette_lighten(lv.PALETTE.LIGHT_BLUE, 5)
fg_color = lv.palette_darken(lv.PALETTE.BLUE, 4)

barcode = lv.barcode(lv.scr_act())
barcode.set_height(50)
barcode.center()

# Set color
barcode.set_dark_color(fg_color)
barcode.set_light_color(bg_color)

# Add a border with bg_color
barcode.set_style_border_color(bg_color, 0)
barcode.set_style_border_width(5, 0)

# Set data
barcode.update("https://lvgl.io")
