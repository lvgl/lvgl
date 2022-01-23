#
# Show how to style a slider.
#
# Create a transition
props = [lv.STYLE.BG_COLOR, 0]
transition_dsc = lv.style_transition_dsc_t()
transition_dsc.init(props, lv.anim_t.path_linear, 300, 0, None)

style_main = lv.style_t()
style_indicator = lv.style_t()
style_knob = lv.style_t()
style_pressed_color = lv.style_t()
style_main.init()
style_main.set_bg_opa(lv.OPA.COVER)
style_main.set_bg_color(lv.color_hex3(0xbbb))
style_main.set_radius(lv.RADIUS.CIRCLE)
style_main.set_pad_ver(-2)                 # Makes the indicator larger

style_indicator.init()
style_indicator.set_bg_opa(lv.OPA.COVER)
style_indicator.set_bg_color(lv.palette_main(lv.PALETTE.CYAN))
style_indicator.set_radius(lv.RADIUS.CIRCLE)
style_indicator.set_transition(transition_dsc)

style_knob.init()
style_knob.set_bg_opa(lv.OPA.COVER)
style_knob.set_bg_color(lv.palette_main(lv.PALETTE.CYAN))
style_knob.set_border_color(lv.palette_darken(lv.PALETTE.CYAN, 3))
style_knob.set_border_width(2)
style_knob.set_radius(lv.RADIUS.CIRCLE)
style_knob.set_pad_all(6)                   # Makes the knob larger
style_knob.set_transition(transition_dsc)

style_pressed_color.init()
style_pressed_color.set_bg_color(lv.palette_darken(lv.PALETTE.CYAN, 2))

# Create a slider and add the style
slider = lv.slider(lv.scr_act())
slider.remove_style_all()                   # Remove the styles coming from the theme

slider.add_style(style_main, lv.PART.MAIN)
slider.add_style(style_indicator, lv.PART.INDICATOR)
slider.add_style(style_pressed_color, lv.PART.INDICATOR | lv.STATE.PRESSED)
slider.add_style(style_knob, lv.PART.KNOB)
slider.add_style(style_pressed_color, lv.PART.KNOB | lv.STATE.PRESSED)

slider.center()

