def event_handler(obj, event):
    if event == lv.EVENT.VALUE_CHANGED:
        print("State: %s" % ("Checked" if obj.is_checked() else "Unchecked"))

cb = lv.cb(lv.scr_act())
cb.set_text("I agree to terms and conditions.")
cb.align(None, lv.ALIGN.CENTER, 0, 0)
cb.set_event_cb(event_handler)