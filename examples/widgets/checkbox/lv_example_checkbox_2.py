import time

class LV_Example_Checkbox_2:
    def __init__(self):
        #
        # Checkboxes as radio buttons
        #
        # The idea is to enable `LV_OBJ_FLAG_EVENT_BUBBLE` on checkboxes and process the
        #`LV.EVENT.CLICKED` on the container.
        # Since user_data cannot be used to pass parameters in MicroPython I use an instance variable to
        # keep the index of the active button

        self.active_index_1 = 0
        self.active_index_2 = 0
        self.style_radio = lv.style_t()
        self.style_radio.init()
        self.style_radio.set_radius(lv.RADIUS_CIRCLE)

        self.style_radio_chk = lv.style_t()
        self.style_radio_chk.init()
        self.style_radio_chk.init()
        self.style_radio_chk.set_bg_img_src(None)

        self.cont1 = lv.obj(lv.scr_act())
        self.cont1.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        self.cont1.set_size(lv.pct(40), lv.pct(80))
        self.cont1.add_event(self.radio_event_handler, lv.EVENT.CLICKED, None)

        for i in range(5):
            txt = "A {:d}".format(i+1)
            self.radiobutton_create(self.cont1,txt)

        # Make the first checkbox checked
        #lv_obj_add_state(lv_obj_get_child(self.cont1, 0), LV_STATE_CHECKED);
        self.cont1.get_child(0).add_state(lv.STATE.CHECKED)

        self.cont2 = lv.obj(lv.scr_act())
        self.cont2.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        self.cont2.set_size(lv.pct(40), lv.pct(80))
        self.cont2.set_x(lv.pct(50))
        self.cont2.add_event(self.radio_event_handler, lv.EVENT.CLICKED, None)

        for i in range(3):
            txt = "B {:d}".format(i+1)
            self.radiobutton_create(self.cont2,txt)

        # Make the first checkbox checked*/
        self.cont2.get_child(0).add_state(lv.STATE.CHECKED)


    def radio_event_handler(self,e):
        cont = e.get_current_target_obj()
        act_cb = e.get_target_obj()
        if cont == self.cont1:
            active_id = self.active_index_1
        else:
            active_id = self.active_index_2
        old_cb = cont.get_child(active_id)

        # Do nothing if the container was clicked
        if act_cb == cont:
            return

        old_cb.clear_state(lv.STATE.CHECKED)          # Uncheck the previous radio button
        act_cb.add_state(lv.STATE.CHECKED)            # Uncheck the current radio button

        if cont == self.cont1:
            self.active_index_1 = act_cb.get_index()
            # print("active index 1: ", self.active_index_1)
        else:
            self.active_index_2 = act_cb.get_index()
            # print("active index 2: ", self.active_index_2)

        print("Selected radio buttons: {:d}, {:d}".format(self.active_index_1, self.active_index_2))

    def radiobutton_create(self,parent, txt):
        obj = lv.checkbox(parent)
        obj.set_text(txt)
        obj.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        obj.add_style(self.style_radio, lv.PART.INDICATOR)
        obj.add_style(self.style_radio_chk, lv.PART.INDICATOR | lv.STATE.CHECKED)

lv_example_checkbox_2 = LV_Example_Checkbox_2()
