#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver

bg_color = lv.palette_lighten(lv.PALETTE.LIGHT_BLUE, 5)
fg_color = lv.palette_darken(lv.PALETTE.BLUE, 4)

qr = lv.qrcode(lv.scr_act())
qr.set_size(150)
qr.set_dark_color(fg_color)
qr.set_light_color(bg_color)

# Set data
data = "https://lvgl.io"
qr.update(data,len(data))
qr.center()

# Add a border with bg_color
qr.set_style_border_color(bg_color, 0)
qr.set_style_border_width(5, 0)
