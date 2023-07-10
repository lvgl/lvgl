#
# Show customizing the circular scrolling animation of a label with `LV_LABEL_LONG_SCROLL_CIRCULAR` long mode.
#

label1 = lv.label(lv.scr_act())
label1.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)         # Circular scroll
label1.set_width(150)
label1.set_text("It is a circularly scrolling text. ")
label1.align(lv.ALIGN.CENTER, 0, 40)

