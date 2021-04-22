def event_handler(obj, event):
    if event == lv.EVENT.VALUE_CHANGED:
        print("Value: %d" % obj.get_value())

# Create styles
style_bg = lv.style_t()
style_indic = lv.style_t()
style_knob = lv.style_t()

lv.style_copy(style_bg, lv.style_pretty)
style_bg.body.main_color =  lv.color_make(0,0,0)
style_bg.body.grad_color =  lv.color_make(0x80, 0x80, 0x80)
style_bg.body.radius = 800 # large enough to make a circle
style_bg.body.border.color = lv.color_make(0xff,0xff,0xff)

lv.style_copy(style_indic, lv.style_pretty_color)
style_indic.body.radius = 800
style_indic.body.shadow.width = 8
style_indic.body.shadow.color = style_indic.body.main_color
style_indic.body.padding.left = 3
style_indic.body.padding.right = 3
style_indic.body.padding.top = 3
style_indic.body.padding.bottom = 3

lv.style_copy(style_knob, lv.style_pretty)
style_knob.body.radius = 800
style_knob.body.opa = lv.OPA._70
style_knob.body.padding.top = 10
style_knob.body.padding.bottom = 10

# Create a slider
slider = lv.slider(lv.scr_act())
slider.set_style(lv.slider.STYLE.BG, style_bg)
slider.set_style(lv.slider.STYLE.INDIC, style_indic)
slider.set_style(lv.slider.STYLE.KNOB, style_knob)
slider.align(None, lv.ALIGN.CENTER, 0, 0)
slider.set_event_cb(event_handler)