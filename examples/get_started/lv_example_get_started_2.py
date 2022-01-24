#
# Create styles from scratch for buttons.
#
style_btn =  lv.style_t()
style_btn_red = lv.style_t()
style_btn_pressed = lv.style_t()

# Create a simple button style
style_btn.init()
style_btn.set_radius(10)
style_btn.set_bg_opa(lv.OPA.COVER)
style_btn.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 3))
style_btn.set_bg_grad_color(lv.palette_main(lv.PALETTE.GREY))
style_btn.set_bg_grad_dir(lv.GRAD_DIR.VER)

# Add a border
style_btn.set_border_color(lv.color_white())
style_btn.set_border_opa(lv.OPA._70)
style_btn.set_border_width(2)

# Set the text style
style_btn.set_text_color(lv.color_white())

# Create a red style. Change only some colors.
style_btn_red.init()
style_btn_red.set_bg_color(lv.palette_main(lv.PALETTE.RED))
style_btn_red.set_bg_grad_color(lv.palette_lighten(lv.PALETTE.RED, 2))

# Create a style for the pressed state.
style_btn_pressed.init()
style_btn_pressed.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
style_btn_pressed.set_bg_grad_color(lv.palette_darken(lv.PALETTE.RED, 3))

# Create a button and use the new styles
btn = lv.btn(lv.scr_act())                  # Add a button the current screen
# Remove the styles coming from the theme
# Note that size and position are also stored as style properties
# so lv_obj_remove_style_all will remove the set size and position too
btn.remove_style_all()                      # Remove the styles coming from the theme
btn.set_pos(10, 10)                         # Set its position
btn.set_size(120, 50)                       # Set its size
btn.add_style(style_btn, 0)
btn.add_style(style_btn_pressed, lv.STATE.PRESSED)

label = lv.label(btn)                       # Add a label to the button
label.set_text("Button")                    # Set the labels text
label.center()

# Create an other button and use the red style too
btn2 = lv.btn(lv.scr_act())
btn2.remove_style_all()                     # Remove the styles coming from the theme
btn2.set_pos(10, 80)                        # Set its position
btn2.set_size(120, 50)                      # Set its size
btn2.add_style(style_btn, 0)
btn2.add_style(style_btn_red, 0)
btn2.add_style(style_btn_pressed, lv.STATE.PRESSED)
btn2.set_style_radius(lv.RADIUS.CIRCLE, 0); # Add a local style

label = lv.label(btn2)                      # Add a label to the button
label.set_text("Button 2");                 # Set the labels text
label.center()

