#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
import fs_driver

info = lv.ft_info_t()
info.name ="./arial.ttf"
info.weight = 24
info.style = lv.FT_FONT_STYLE.NORMAL
info.font_init()

# Create style with the new font
style = lv.style_t()
style.init()
style.set_text_font(info.font)
style.set_text_align(lv.TEXT_ALIGN.CENTER)

# Create a label with the new style
label = lv.label(lv.scr_act())
label.add_style(style, 0)
label.set_text("Hello world\nI'm a font created with FreeType")
label.center()
