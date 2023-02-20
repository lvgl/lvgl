def ta_event_cb(e):
    code = e.get_code()
    ta = e.get_target_obj()
    if code == lv.EVENT.CLICKED or code == lv.EVENT.FOCUSED:
        # Focus on the clicked text area
        if kb != None:
            kb.set_textarea(ta)

    elif code == lv.EVENT.READY:
        print("Ready, current text: " + ta.get_text())


# Create the password box

pwd_ta = lv.textarea(lv.scr_act())
pwd_ta.set_text("")
pwd_ta.set_password_mode(True)
pwd_ta.set_one_line(True)
pwd_ta.set_width(lv.pct(45))
pwd_ta.set_pos(5, 20)
pwd_ta.add_event(ta_event_cb, lv.EVENT.ALL, None)

# Create a label and position it above the text box
pwd_label = lv.label(lv.scr_act())
pwd_label.set_text("Password:")
pwd_label.align_to(pwd_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create the one-line mode text area
text_ta = lv.textarea(lv.scr_act())
text_ta.set_width(lv.pct(45))
text_ta.set_one_line(True)
text_ta.add_event(ta_event_cb, lv.EVENT.ALL, None)
text_ta.set_password_mode(False)

text_ta.align(lv.ALIGN.TOP_RIGHT, -5, 20)

# Create a label and position it above the text box
oneline_label = lv.label(lv.scr_act())
oneline_label.set_text("Text:")
oneline_label.align_to(text_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create a keyboard
kb = lv.keyboard(lv.scr_act())
kb.set_size(lv.pct(100), lv.pct(50))

kb.set_textarea(pwd_ta)  # Focus it on one of the text areas to start

