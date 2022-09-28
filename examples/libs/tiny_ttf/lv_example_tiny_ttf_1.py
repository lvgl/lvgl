from ubuntu_font import ubuntu_font
# 
# Load a font with Tiny_TTF
#
#Create style with the new font
style = lv.style_t()
style.init()
font = lv.tiny_ttf_create_data(ubuntu_font, len(ubuntu_font), 30)
style.set_text_font(font)
style.set_text_align(lv.TEXT_ALIGN.CENTER)

# Create a label with the new style
label = lv.label(lv.scr_act())
label.add_style(style, 0)
label.set_text("Hello world\nI'm a font created with Tiny TTF")
label.center()
