#
# Create styles from scratch for buttons.
#
style_button =  lv.style_t()
style_button_red = lv.style_t()
style_button_pressed = lv.style_t()

# Create a simple button style
style_button.init()
style_button.set_radius(10)
style_button.set_bg_opa(lv.OPA.COVER)
style_button.set_bg_color(lv.palette_lighten(lv.PALETTE.GREY, 3))
style_button.set_bg_grad_color(lv.palette_main(lv.PALETTE.GREY))
style_button.set_bg_grad_dir(lv.GRAD_DIR.VER)

# Add a border
style_button.set_border_color(lv.color_white())
style_button.set_border_opa(lv.OPA._70)
style_button.set_border_width(2)

# Set the text style
style_button.set_text_color(lv.color_white())

# Create a red style. Change only some colors.
style_button_red.init()
style_button_red.set_bg_color(lv.palette_main(lv.PALETTE.RED))
style_button_red.set_bg_grad_color(lv.palette_lighten(lv.PALETTE.RED, 2))

# Create a style for the pressed state.
style_button_pressed.init()
style_button_pressed.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
style_button_pressed.set_bg_grad_color(lv.palette_darken(lv.PALETTE.RED, 3))

# Create a button and use the new styles
button = lv.button(lv.scr_act())                  # Add a button the current screen
# Remove the styles coming from the theme
# Note that size and position are also stored as style properties
# so lv_obj_remove_style_all will remove the set size and position too
button.remove_style_all()                      # Remove the styles coming from the theme
button.set_pos(10, 10)                         # Set its position
button.set_size(120, 50)                       # Set its size
button.add_style(style_button, 0)
button.add_style(style_button_pressed, lv.STATE.PRESSED)

label = lv.label(button)                       # Add a label to the button
label.set_text("Button")                    # Set the labels text
label.center()

# Create a slider in the center of the display
slider = lv.slider(lv.scr_act())
slider.set_width(200)                                              # Set the width
slider.center()                                                    # Align to the center of the parent (screen)

# Create another button and use the red style too
button2 = lv.button(lv.scr_act())
button2.remove_style_all()                     # Remove the styles coming from the theme
button2.set_pos(10, 80)                        # Set its position
button2.set_size(120, 50)                      # Set its size
button2.add_style(style_button, 0)
button2.add_style(style_button_red, 0)
button2.add_style(style_button_pressed, lv.STATE.PRESSED)
button2.set_style_radius(lv.RADIUS_CIRCLE, 0)  # Add a local style

label = lv.label(button2)                      # Add a label to the button
label.set_text("Button 2")                  # Set the labels text
label.center()

