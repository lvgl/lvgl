def slider_event_cb(slider, event):
    if event == lv.EVENT.VALUE_CHANGED:
        slider_label.set_text("%u" % slider.get_value())

# Create a slider in the center of the display
slider = lv.slider(lv.scr_act())
slider.set_width(200)
slider.align(None, lv.ALIGN.CENTER, 0, 0)
slider.set_event_cb(slider_event_cb)
slider.set_range(0, 100)

# Create a label below the slider
slider_label = lv.label(lv.scr_act())
slider_label.set_text("0")
slider_label.set_auto_realign(True)
slider_label.align(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

# Create an informative label
info = lv.label(lv.scr_act())
info.set_text("""Welcome to the slider+label demo!
Move the slider and see that the label
updates to match it.""")
info.align(None, lv.ALIGN.IN_TOP_LEFT, 10, 10)
