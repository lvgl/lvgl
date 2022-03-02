def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.add_flag(lv.obj.FLAG.HIDDEN)
        
# Create a keyboard to use it with one of the text areas
kb = lv.keyboard(lv.scr_act())

# Create a text area. The keyboard will write here
ta = lv.textarea(lv.scr_act())
ta.set_width(200)
ta.align(lv.ALIGN.TOP_LEFT, 10, 10)
ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)
ta.set_placeholder_text("Hello")

ta = lv.textarea(lv.scr_act())
ta.set_width(200)
ta.align(lv.ALIGN.TOP_RIGHT, -10, 10)
ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)

kb.set_textarea(ta)

