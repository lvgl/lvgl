def event_cb(e):
    btn = e.get_target_obj()
    label = btn.get_child(btn)
    print("Button %s clicked" % label.get_text())

mbox1 = lv.msgbox(lv.screen_active())
mbox1.add_title("Hello")
mbox1.add_text("This is a message box with two buttons")
mbox1.add_close_button()

btn = mbox1.add_footer_button("Apply")
btn.add_event_cb(event_cb, lv.EVENT.CLICKED, None)
btn = mbox1.add_footer_button("Cancel")
btn.add_event_cb(event_cb, lv.EVENT.CLICKED, None)
