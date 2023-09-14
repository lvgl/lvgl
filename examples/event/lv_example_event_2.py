def event_cb(e,label):
    code = e.get_code()
    if code == lv.EVENT.PRESSED:
        label.set_text("The last button event:\nLV_EVENT_PRESSED")
    elif code == lv.EVENT.CLICKED:
        label.set_text("The last button event:\nLV_EVENT_CLICKED")
    elif code == lv.EVENT.LONG_PRESSED:
        label.set_text("The last button event:\nLV_EVENT_LONG_PRESSED")
    elif code == lv.EVENT.LONG_PRESSED_REPEAT:
        label.set_text("The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT")
button = lv.button(lv.scr_act())
button.set_size(100, 50)
button.center()

button_label = lv.label(button)
button_label.set_text("Click me!")
button_label.center()

info_label = lv.label(lv.scr_act())
info_label.set_text("The last button event:\nNone")

button.add_event(lambda e: event_cb(e,info_label), lv.EVENT.ALL, None)
