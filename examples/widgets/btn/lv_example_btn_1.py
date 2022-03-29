def event_handler(evt):
    code = evt.get_code()

    if code == lv.EVENT.CLICKED:
            print("Clicked event seen")
    elif code == lv.EVENT.VALUE_CHANGED:
        print("Value changed seen")

# create a simple button
btn1 = lv.btn(lv.scr_act())

# attach the callback
btn1.add_event_cb(event_handler,lv.EVENT.ALL, None)

btn1.align(lv.ALIGN.CENTER,0,-40)
label=lv.label(btn1)
label.set_text("Button")

# create a toggle button
btn2 = lv.btn(lv.scr_act())

# attach the callback
#btn2.add_event_cb(event_handler,lv.EVENT.VALUE_CHANGED,None)
btn2.add_event_cb(event_handler,lv.EVENT.ALL, None)

btn2.align(lv.ALIGN.CENTER,0,40)
btn2.add_flag(lv.obj.FLAG.CHECKABLE)
btn2.set_height(lv.SIZE.CONTENT)

label=lv.label(btn2)
label.set_text("Toggle")
label.center()
