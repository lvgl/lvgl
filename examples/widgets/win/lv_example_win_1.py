def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.CLICKED:
        print("Button {:d} clicked".format(obj.get_child_id()))


win = lv.win(lv.scr_act(), 60)
btn1 = win.add_btn(lv.SYMBOL.LEFT, 40)
btn1.add_event_cb(event_handler, lv.EVENT.ALL, None)
win.add_title("A title")
btn2=win.add_btn(lv.SYMBOL.RIGHT, 40)
btn2.add_event_cb(event_handler, lv.EVENT.ALL, None)
btn3 = win.add_btn(lv.SYMBOL.CLOSE, 60)
btn3.add_event_cb(event_handler, lv.EVENT.ALL, None)

cont = win.get_content()  # Content can be added here
label = lv.label(cont)
label.set_text("""This is
a pretty
long text
to see how
the window
becomes
scrollable.


We need
quite some text
and we will 
even put
some more
text to be
sure it
overflows.
""")
