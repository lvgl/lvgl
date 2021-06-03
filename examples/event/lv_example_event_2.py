def event_cb(e,label):
    code = e.get_code()
    if code == lv.EVENT.PRESSED:
        label.set_text("The last button event:\nLV_EVENT_PRESSED")
    elif code == lv.EVENT.CLICKED:
        label.set_text("The last button event:\nLV_EVENT_CLICKED")
    elif code ==  lv.EVENT.LONG_PRESSED:
        label.set_text("The last button event:\nLV_EVENT_LONG_PRESSED")
    elif code == lv.EVENT.LONG_PRESSED_REPEAT:
        label.set_text("The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT")        
btn = lv.btn(lv.scr_act())
btn.set_size(100, 50)
btn.center()

btn_label = lv.label(btn)
btn_label.set_text("Click me!")
btn_label.center()

info_label = lv.label(lv.scr_act())
info_label.set_text("The last button event:\nNone");

btn.add_event_cb(lambda e: event_cb(e,info_label), lv.EVENT.ALL, None)
