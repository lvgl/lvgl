def event_handler(obj, event):
    if event == lv.EVENT.VALUE_CHANGED:
        print("Value: %d" % obj.get_value())
    elif event == lv.EVENT.CLICKED:
        # For simple test: Click the spinbox to increment its value
        obj.increment()

spinbox = lv.spinbox(lv.scr_act())
spinbox.set_digit_format(5, 3)
spinbox.step_prev()
spinbox.set_width(100)
spinbox.align(None, lv.ALIGN.CENTER, 0, 0)
spinbox.set_event_cb(event_handler)