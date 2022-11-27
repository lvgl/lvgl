import fs_driver

# needed for dynamic font loading
fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'S')

# get the directory in which the script is running
try:
    script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
except NameError: 
    script_path = ''

#
# Load a font with Tiny_TTF from file
#
# Create style with the new font
style = lv.style_t()
style.init()
font = lv.tiny_ttf_create_file("S:" + script_path + "/Ubuntu-Medium.ttf", 30)
style.set_text_font(font)
style.set_text_align(lv.TEXT_ALIGN.CENTER)

# Create a label with the new style
label = lv.label(lv.scr_act())
label.add_style(style, 0)
label.set_text("Hello world\nI'm a font created with Tiny TTF")
label.center()
