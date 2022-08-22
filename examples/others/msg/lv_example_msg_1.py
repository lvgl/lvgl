
# Define a message ID
MSG_NEW_TEMPERATURE = const(1)

# Define the object that will be sent as msg payload
class Temperature:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return f"{self.value} °C"

def slider_event_cb(e):
    slider = e.get_target()
    v = slider.get_value()
    # Notify all subscribers (only the label now) that the slider value has been changed
    lv.msg_send(MSG_NEW_TEMPERATURE, Temperature(v))

def label_event_cb(e):
    label = e.get_target()
    msg = e.get_msg()
    # Respond only to MSG_NEW_TEMPERATURE message
    if msg.get_id() == MSG_NEW_TEMPERATURE:
        payload = msg.get_payload()
        temprature = payload.__cast__()
        label.set_text(str(temprature))

# Create a slider in the center of the display
slider = lv.slider(lv.scr_act())
slider.center()
slider.add_event_cb(slider_event_cb, lv.EVENT.VALUE_CHANGED, None)

# Create a label below the slider
label = lv.label(lv.scr_act())
label.add_event_cb(label_event_cb, lv.EVENT.MSG_RECEIVED, None)
label.set_text("0%")
label.align(lv.ALIGN.CENTER, 0, 30)

# Subscribe the label to a message
lv.msg_subscribe_obj(MSG_NEW_TEMPERATURE, label, None)
