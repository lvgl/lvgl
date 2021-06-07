#
# Add styles to parts and states
#

style_indic = lv.style_t()
style_indic.init()
style_indic.set_bg_color(lv.palette_lighten(lv.PALETTE.RED, 3))
style_indic.set_bg_grad_color(lv.palette_main(lv.PALETTE.RED))
style_indic.set_bg_grad_dir(lv.GRAD_DIR.HOR)

style_indic_pr = lv.style_t()
style_indic_pr.init()
style_indic_pr.set_shadow_color(lv.palette_main(lv.PALETTE.RED))
style_indic_pr.set_shadow_width(10)
style_indic_pr.set_shadow_spread(3)

# Create an object with the new style_pr
obj = lv.slider(lv.scr_act())
obj.add_style(style_indic, lv.PART.INDICATOR)
obj.add_style(style_indic_pr, lv.PART.INDICATOR | lv.STATE.PRESSED)
obj.set_value(70, lv.ANIM.OFF)
obj.center()

