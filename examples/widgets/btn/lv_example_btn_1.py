def event_handler(evt):
    code = evt.get_code()

    if code == lv.EVENT.CLICKED:
            print("Clicked event seen")
    elif code == lv.EVENT.VALUE_CHANGED:
        print("Value changed seen")

# create a simple button
button1 = lv.button(lv.scr_act())

# attach the callback
button1.add_event(event_handler,lv.EVENT.ALL, None)

button1.align(lv.ALIGN.CENTER,0,-40)
label=lv.label(button1)
label.set_text("Button")

# create a toggle button
button2 = lv.button(lv.scr_act())

# attach the callback
#button2.add_event(event_handler,lv.EVENT.VALUE_CHANGED,None)
button2.add_event(event_handler,lv.EVENT.ALL, None)

button2.align(lv.ALIGN.CENTER,0,40)
button2.add_flag(lv.obj.FLAG.CHECKABLE)
button2.set_height(lv.SIZE_CONTENT)

label=lv.label(button2)
label.set_text("Toggle")
label.center()
