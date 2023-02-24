def ta_event_cb(e):
    ta = e.get_target_obj()
    txt = ta.get_text()
    # print(txt)
    pos = ta.get_cursor_pos()
    # print("cursor pos: ",pos)
    # find position of ":" in text
    colon_pos= txt.find(":")
    # if there are more than 2 digits before the colon, remove the last one entered
    if colon_pos == 3:
        ta.del_char()
    if colon_pos != -1:
        # if there are more than 3 digits after the ":" remove the last one entered
        rest = txt[colon_pos:]
        if len(rest) > 3:
            ta.del_char()

    if len(txt) < 2:
        return
    if ":" in txt:
        return
    if  txt[0] >= '0' and txt[0] <= '9' and \
        txt[1] >= '0' and txt[1] <= '9':
        if len(txt) == 2 or txt[2] != ':' :
            ta.set_cursor_pos(2)
            ta.add_char(ord(':'))
#
# Automatically format text like a clock. E.g. "12:34"
# Add the ':' automatically
#
# Create the text area

ta = lv.textarea(lv.scr_act())
ta.add_event(ta_event_cb, lv.EVENT.VALUE_CHANGED, None)
ta.set_accepted_chars("0123456789:")
ta.set_max_length(5)
ta.set_one_line(True)
ta.set_text("")
ta.add_state(lv.STATE.FOCUSED)

# Create a keyboard
kb = lv.keyboard(lv.scr_act())
kb.set_size(lv.pct(100), lv.pct(50))
kb.set_mode(lv.keyboard.MODE.NUMBER)
kb.set_textarea(ta)


