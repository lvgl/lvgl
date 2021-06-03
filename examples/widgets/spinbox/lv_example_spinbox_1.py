def increment_event_cb(e):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code  == lv.EVENT.LONG_PRESSED_REPEAT:
        spinbox.increment()

def decrement_event_cb(e):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        spinbox.decrement()

spinbox = lv.spinbox(lv.scr_act())
spinbox.set_range(-1000, 25000)
spinbox.set_digit_format(5, 2)
spinbox.step_prev()
spinbox.set_width(100)
spinbox.center()

h = spinbox.get_height()

btn = lv.btn(lv.scr_act())
btn.set_size(h, h)
btn.align_to(spinbox, lv.ALIGN.OUT_RIGHT_MID, 5, 0)
btn.set_style_bg_img_src(lv.SYMBOL.PLUS, 0)
btn.add_event_cb(increment_event_cb, lv.EVENT.ALL,  None)

btn = lv.btn(lv.scr_act())
btn.set_size(h, h)
btn.align_to(spinbox, lv.ALIGN.OUT_LEFT_MID, -5, 0)
btn.set_style_bg_img_src(lv.SYMBOL.MINUS, 0)
btn.add_event_cb(decrement_event_cb, lv.EVENT.ALL, None)
