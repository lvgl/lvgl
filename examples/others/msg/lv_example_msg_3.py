# Define a message ID
MSG_INC            = 1
MSG_DEC            = 2
MSG_SET            = 3
MSG_UPDATE         = 4
MSG_UPDATE_REQUEST = 5

# Define the object that will be sent as msg payload
class NewValue:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return f"{self.value} %"
class LV_Example_Msg_2:

    def __init__(self):
        self.value = 10
        lv.msg_subscribe(MSG_INC, self.value_handler, None)
        lv.msg_subscribe(MSG_DEC, self.value_handler, None)
        lv.msg_subscribe(MSG_SET, self.value_handler, None)
        lv.msg_subscribe(MSG_UPDATE, self.value_handler, None)
        lv.msg_subscribe(MSG_UPDATE_REQUEST, self.value_handler, None)

        panel = lv.obj(lv.scr_act())
        panel.set_size(250, lv.SIZE_CONTENT)
        panel.center()
        panel.set_flex_flow(lv.FLEX_FLOW.ROW)
        panel.set_flex_align(lv.FLEX_ALIGN.SPACE_BETWEEN, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START)

        # Up button
        btn = lv.btn(panel)
        btn.set_flex_grow(1)
        btn.add_event(self.btn_event_cb, lv.EVENT.ALL, None)
        label = lv.label(btn)
        label.set_text(lv.SYMBOL.LEFT)
        label.center()

        # Current value
        label = lv.label(panel)
        label.set_flex_grow(2)
        label.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
        label.set_text("?")
        lv.msg_subscribe_obj(MSG_UPDATE, label, None)
        label.add_event(self.label_event_cb, lv.EVENT.MSG_RECEIVED, None)

        # Down button
        btn = lv.btn(panel)
        btn.set_flex_grow(1)
        btn.add_event(self.btn_event_cb, lv.EVENT.ALL, None)
        label = lv.label(btn)
        label.set_text(lv.SYMBOL.RIGHT)
        label.center()

        # Slider
        slider = lv.slider(panel)
        slider.set_flex_grow(1)
        slider.add_flag(lv.OBJ_FLAG_FLEX_IN_NEW_TRACK)
        slider.add_event(self.slider_event_cb, lv.EVENT.ALL, None)
        lv.msg_subscribe_obj(MSG_UPDATE, slider, None)


        # As there are new UI elements that don't know the system's state
        # send an UPDATE REQUEST message which will trigger an UPDATE message with the current value
        lv.msg_send(MSG_UPDATE_REQUEST, None)

    def value_handler(self,m):
        old_value = self.value
        id = m.get_id()
        if id == MSG_INC:
            if self.value < 100:
                self.value +=1
        elif id == MSG_DEC:
            if self.value > 0:
                self.value -=1
        elif id == MSG_SET:
            payload = m.get_payload()
            new_value=payload.__cast__()
            self.value = new_value.value
            # print("value_handler: new value: {:d}".format(new_value.value))
        elif id == MSG_UPDATE_REQUEST:
            lv.msg_send(MSG_UPDATE, NewValue(self.value))

        if self.value != old_value:
                lv.msg_send(MSG_UPDATE, NewValue(self.value));

    def btn_event_cb(self,e):
        btn = e.get_target_obj()
        code = e.get_code()
        if code == lv.EVENT.CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
            if btn.get_index() == 0:      # rst object is the dec. button
                lv.msg_send(MSG_DEC, None)
            else :
                lv.msg_send(MSG_INC, None)

    def label_event_cb(self,e):
        label = e.get_target_obj()
        code = e.get_code()
        if code == lv.EVENT.MSG_RECEIVED:
            m = e.get_msg()
            if m.get_id() == MSG_UPDATE:
                payload = m.get_payload()
                value=payload.__cast__()
                # print("label_event_cb: " + str(value))
                label.set_text(str(value))

    def slider_event_cb(self,e):
        slider = e.get_target_obj()
        code = e.get_code()
        if code == lv.EVENT.VALUE_CHANGED:
            v = slider.get_value()
            # print("slider_event_cb: {:d}".format(v))
            lv.msg_send(MSG_SET, NewValue(v))

        elif code == lv.EVENT.MSG_RECEIVED:
            m = e.get_msg()
            if m.get_id() == MSG_UPDATE:
                v = m.get_payload()
                value = v.__cast__()
                slider.set_value(value.value, lv.ANIM.OFF)

lv_example_msg_2 = LV_Example_Msg_2()

