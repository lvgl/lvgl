def event_handler(obj, event):
    if event == lv.EVENT.CLICKED:
        print("Clicked")

btn1 = lv.btn(lv.scr_act())
btn1.set_event_cb(event_handler)
btn1.align(None, lv.ALIGN.CENTER, 0, -40)

label = lv.label(btn1)
label.set_text("Button")

btn2 = lv.btn(lv.scr_act())
# callback can be lambda:
btn2.set_event_cb(lambda obj, event: print("Toggled") if event == lv.EVENT.VALUE_CHANGED else None)
btn2.align(None, lv.ALIGN.CENTER, 0, 40)
btn2.set_toggle(True)
btn2.toggle()
btn2.set_fit2(lv.FIT.NONE, lv.FIT.TIGHT)

label = lv.label(btn2)
label.set_text("Toggled")
