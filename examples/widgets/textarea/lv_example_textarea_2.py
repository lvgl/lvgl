HOR_RES = lv.disp_get_hor_res(lv.disp_get_default())

def kb_event_cb(event_kb, event):
    # Just call the regular event handler
    event_kb.def_event_cb(event)

def ta_event_cb(ta, event):
    if event == lv.EVENT.INSERT:
        # get inserted value
        ptr = lv.C_Pointer()
        ptr.ptr_val = lv.event_get_data()
        if ptr.str_val == "\n":
            print("Ready")
    elif event == lv.EVENT.CLICKED:
        # Focus on the clicked text area
        kb.set_ta(ta)

# Create the password box
pwd_ta = lv.ta(lv.scr_act())
pwd_ta.set_text("");
pwd_ta.set_pwd_mode(True)
pwd_ta.set_one_line(True)
pwd_ta.set_width(HOR_RES // 2 - 20)
pwd_ta.set_pos(5, 20)
pwd_ta.set_event_cb(ta_event_cb)

# Create a label and position it above the text box
pwd_label = lv.label(lv.scr_act())
pwd_label.set_text("Password:")
pwd_label.align(pwd_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create the one-line mode text area
oneline_ta = lv.ta(lv.scr_act(), pwd_ta)
oneline_ta.set_pwd_mode(False)
oneline_ta.set_cursor_type(lv.CURSOR.LINE | lv.CURSOR.HIDDEN)
oneline_ta.align(None, lv.ALIGN.IN_TOP_RIGHT, -5, 20)
oneline_ta.set_event_cb(ta_event_cb)

# Create a label and position it above the text box
oneline_label = lv.label(lv.scr_act())
oneline_label.set_text("Text:")
oneline_label.align(oneline_ta, lv.ALIGN.OUT_TOP_LEFT, 0, 0)

# Create a keyboard and make it fill the width of the above text areas
kb = lv.kb(lv.scr_act())
kb.set_pos(5, 90)
kb.set_event_cb(kb_event_cb) # Setting a custom event handler stops the keyboard from closing automatically
kb.set_size(HOR_RES - 10, 140)

kb.set_ta(pwd_ta) # Focus it on one of the text areas to start
kb.set_cursor_manage(True) # Automatically show/hide cursors on text areas