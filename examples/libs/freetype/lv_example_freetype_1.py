#!/opt/bin/lv_micropython -i
import lvgl as lv
import display_driver
import fs_driver

font = lv.freetype_font_create("./Lato-Regular.ttf", 24, lv.FREETYPE_FONT_STYLE.NORMAL)

# Create style with the new font
style = lv.style_t()
style.init()
style.set_text_font(font)
style.set_text_align(lv.TEXT_ALIGN.CENTER)

# Create a label with the new style
label = lv.label(lv.scr_act())
label.add_style(style, 0)
label.set_text("Hello world\nI'm a font created with FreeType")
label.center()
