from ubuntu_font import ubuntu_font

DISPLAY_TEXT = "Hello World!"
MSG_NEW_SIZE = 1

#
# Change font size with Tiny_TTF
#

lsize = 25

# Define the object that will be sent as msg payload
class NewValue:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return f"{self.value} %"
    
def slider_event_cb(e,label):

    slider = e.get_target_obj()
    lsize = slider.get_value()
    label.set_text("{:d}".format(lsize))
    lv.msg_send(MSG_NEW_SIZE, NewValue(lsize))


def label_event_cb(e,font):

    label = e.get_target_obj()
    m = e.get_msg()
    payload  = m.get_payload()
    v = payload.__cast__()
    lv.tiny_ttf_set_size(font, v.value)
    label.set_text(DISPLAY_TEXT)

# Create style with the new font
style = lv.style_t()
style.init()
font = lv.tiny_ttf_create_data(ubuntu_font, len(ubuntu_font),lsize)
style.set_text_font(font)
style.set_text_align(lv.TEXT_ALIGN.CENTER)

slider = lv.slider(lv.scr_act())
slider.center()
slider.set_range(5, 50)
slider.set_value(lsize, lv.ANIM.OFF)
slider.align(lv.ALIGN.BOTTOM_MID, 0, -50)

slider_label = lv.label(lv.scr_act())
slider_label.set_text("{:d}".format(lsize))
slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

slider.add_event(lambda e: slider_event_cb(e,slider_label), lv.EVENT.VALUE_CHANGED, None)

# Create a label with the new style
label = lv.label(lv.scr_act())
label.add_style(style, 0)
label.add_event(lambda e: label_event_cb(e,font), lv.EVENT.MSG_RECEIVED, None)
label.set_size(lv.SIZE_CONTENT, lv.SIZE_CONTENT)
label.center()

lv.msg_subscribe_obj(MSG_NEW_SIZE, label, None)
lv.msg_send(MSG_NEW_SIZE, NewValue(lsize))


