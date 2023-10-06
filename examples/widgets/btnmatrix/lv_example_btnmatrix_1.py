def event_handler(e):
    code = e.get_code()
    obj  = e.get_target_obj()

    if code == lv.EVENT.VALUE_CHANGED :
        id = obj.get_selected_button()
        txt = obj.get_button_text(id)

        print("%s was pressed"%txt)

buttonm_map = ["1", "2", "3", "4", "5", "\n",
            "6", "7", "8", "9", "0", "\n",
            "Action1", "Action2", ""]

buttonm1 = lv.buttonmatrix(lv.scr_act())
buttonm1.set_map(buttonm_map)
buttonm1.set_button_width(10, 2)        # Make "Action1" twice as wide as "Action2"
buttonm1.set_button_ctrl(10, lv.buttonmatrix.CTRL.CHECKABLE)
buttonm1.set_button_ctrl(11, lv.buttonmatrix.CTRL.CHECKED)
buttonm1.align(lv.ALIGN.CENTER, 0, 0)
buttonm1.add_event(event_handler, lv.EVENT.ALL, None)


#endif
