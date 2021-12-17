def slider_event_cb(evt):
    slider = evt.get_target()

    # Refresh the text
    label.set_text(str(slider.get_value()))

#
# Create a slider and write its value on a label.
#
            
# Create a slider in the center of the display
slider = lv.slider(lv.scr_act())
slider.set_width(200)                                              # Set the width
slider.center()                                                    # Align to the center of the parent (screen)
slider.add_event_cb(slider_event_cb, lv.EVENT.VALUE_CHANGED, None) # Assign an event function

# Create a label below the slider
label = lv.label(lv.scr_act())
label.set_text("0")
label.align_to(slider, lv.ALIGN.OUT_TOP_MID, 0, -15)               # Align below the slider


