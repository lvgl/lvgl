def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.CLICKED:
        print("Button {:d} clicked".format(obj.get_index()))


win = lv.win(lv.scr_act())
button1 = win.add_button(lv.SYMBOL.LEFT, 40)
button1.add_event(event_handler, lv.EVENT.ALL, None)
win.add_title("A title")
button2=win.add_button(lv.SYMBOL.RIGHT, 40)
button2.add_event(event_handler, lv.EVENT.ALL, None)
button3 = win.add_button(lv.SYMBOL.CLOSE, 60)
button3.add_event(event_handler, lv.EVENT.ALL, None)

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
