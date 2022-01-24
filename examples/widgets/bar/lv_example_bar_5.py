#
# Bar with LTR and RTL base direction
#

bar_ltr = lv.bar(lv.scr_act())
bar_ltr.set_size(200, 20)
bar_ltr.set_value(70, lv.ANIM.OFF)
bar_ltr.align(lv.ALIGN.CENTER, 0, -30)

label = lv.label(lv.scr_act())
label.set_text("Left to Right base direction")
label.align_to(bar_ltr, lv.ALIGN.OUT_TOP_MID, 0, -5)

bar_rtl = lv.bar(lv.scr_act())
bar_rtl.set_style_base_dir(lv.BASE_DIR.RTL,0)
bar_rtl.set_size(200, 20)
bar_rtl.set_value(70, lv.ANIM.OFF)
bar_rtl.align(lv.ALIGN.CENTER, 0, 30)

label = lv.label(lv.scr_act())
label.set_text("Right to Left base direction")
label.align_to(bar_rtl, lv.ALIGN.OUT_TOP_MID, 0, -5)
