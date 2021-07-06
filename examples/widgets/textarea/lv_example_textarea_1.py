def textarea_event_handler(e,ta):
    print("Enter was pressed. The current text is: " + ta.get_text())
    
def btnm_event_handler(e,ta):

    obj = e.get_target()
    txt = obj.get_btn_text(obj.get_selected_btn())
    if txt == lv.SYMBOL.BACKSPACE:
        ta.del_char()
    elif txt == lv.SYMBOL.NEW_LINE:
        lv.event_send(ta,lv.EVENT.READY,None)
    elif txt:
        ta.add_text(txt)

ta = lv.textarea(lv.scr_act())
ta.set_one_line(True)
ta.align(lv.ALIGN.TOP_MID, 0, 10)
ta.add_event_cb(lambda e: textarea_event_handler(e,ta), lv.EVENT.READY, None)
ta.add_state(lv.STATE.FOCUSED)   # To be sure the cursor is visible

btnm_map = ["1", "2", "3", "\n",
            "4", "5", "6", "\n",
            "7", "8", "9", "\n",
            lv.SYMBOL.BACKSPACE, "0", lv.SYMBOL.NEW_LINE, ""]
         
btnm = lv.btnmatrix(lv.scr_act())
btnm.set_size(200, 150)
btnm.align(lv.ALIGN.BOTTOM_MID, 0, -10)
btnm.add_event_cb(lambda e: btnm_event_handler(e,ta), lv.EVENT.VALUE_CHANGED, None)
btnm.clear_flag(lv.obj.FLAG.CLICK_FOCUSABLE)    # To keep the text area focused on button clicks
btnm.set_map(btnm_map)

