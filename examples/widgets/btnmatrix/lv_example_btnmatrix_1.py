def event_handler(obj, event):
    if event == lv.EVENT.VALUE_CHANGED:
        txt = obj.get_active_btn_text()
        print("%s was pressed" % txt)

btnm_map = ["1", "2", "3", "4", "5", "\n",
            "6", "7", "8", "9", "0", "\n",
            "Action1", "Action2", ""]

btnm1 = lv.btnm(lv.scr_act())
btnm1.set_map(btnm_map)
btnm1.set_btn_width(10, 2)        # Make "Action1" twice as wide as "Action2"
btnm1.align(None, lv.ALIGN.CENTER, 0, 0)
btnm1.set_event_cb(event_handler)