def textarea_event_handler(e, ta):
    print("Enter was pressed. The current text is: " + ta.get_text())


def buttonm_event_handler(e, ta):
    obj = e.get_target_obj()
    txt = obj.get_button_text(obj.get_selected_button())
    if txt == lv.SYMBOL.BACKSPACE:
        ta.del_char()
    elif txt == lv.SYMBOL.NEW_LINE:
        lv.event_send(ta, lv.EVENT.READY, None)
    elif txt:
        ta.add_text(txt)


ta = lv.textarea(lv.scr_act())
ta.set_one_line(True)
ta.align(lv.ALIGN.TOP_MID, 0, 10)
ta.add_event(lambda e: textarea_event_handler(e, ta), lv.EVENT.READY, None)
ta.add_state(lv.STATE.FOCUSED)   # To be sure the cursor is visible

buttonm_map = ["1", "2", "3", "\n",
            "4", "5", "6", "\n",
            "7", "8", "9", "\n",
            lv.SYMBOL.BACKSPACE, "0", lv.SYMBOL.NEW_LINE, ""]

buttonm = lv.buttonmatrix(lv.scr_act())
buttonm.set_size(200, 150)
buttonm.align(lv.ALIGN.BOTTOM_MID, 0, -10)
buttonm.add_event(lambda e: buttonm_event_handler(e, ta), lv.EVENT.VALUE_CHANGED, None)
buttonm.clear_flag(lv.obj.FLAG.CLICK_FOCUSABLE)    # To keep the text area focused on button clicks
buttonm.set_map(buttonm_map)
