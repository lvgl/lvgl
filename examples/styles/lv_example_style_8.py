#
# Using the text style properties
#

style = lv.style_t()
style.init()

style.set_radius(5)
style.set_bg_opa(lv.OPA.COVER)
style.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 3))
style.set_border_width(2)
style.set_border_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_pad_all(10)

style.set_text_color(lv.palette_main(lv.PALETTE.BLUE))
style.set_text_letter_space(5)
style.set_text_line_space(20)
style.set_text_decor(lv.TEXT_DECOR.UNDERLINE)

# Create an object with the new style
obj = lv.label(lv.scr_act())
obj.add_style(style, 0)
obj.set_text("Text of\n"
             "a label");

obj.center()

