#
# Example of styling the bar
#
style_bg = lv.style_t()
style_indic = lv.style_t()

style_bg.init()
style_bg.set_border_color(lv.palette_main(lv.PALETTE.BLUE))
style_bg.set_border_width(2)
style_bg.set_pad_all(6)            # To make the indicator smaller
style_bg.set_radius(6)
style_bg.set_anim_time(1000)

style_indic.init()
style_indic.set_bg_opa(lv.OPA.COVER)
style_indic.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
style_indic.set_radius(3)

bar = lv.bar(lv.scr_act())
bar.remove_style_all()   # To have a clean start
bar.add_style(style_bg, 0)
bar.add_style(style_indic, lv.PART.INDICATOR)

bar.set_size(200, 20)
bar.center()
bar.set_value(100, lv.ANIM.ON)

