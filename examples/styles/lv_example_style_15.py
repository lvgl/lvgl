#
# Opacity and Transformations
#

# Normal button
button = lv.button(lv.scr_act())
button.set_size(100, 40)
button.align(lv.ALIGN.CENTER, 0, -70)

label = lv.label(button)
label.set_text("Normal")
label.center()

# Set opacity
# The button and the label is rendered to a layer first and that layer is blended
button = lv.button(lv.scr_act())
button.set_size(100, 40)
button.set_style_opa(lv.OPA._50, 0)
button.align(lv.ALIGN.CENTER, 0, 0)

label = lv.label(button)
label.set_text("Opa:50%")
label.center()

# Set transformations
# The button and the label is rendered to a layer first and that layer is transformed
button = lv.button(lv.scr_act())
button.set_size(100, 40)
button.set_style_transform_angle(150, 0)             # 15 deg
button.set_style_transform_zoom(256 + 64, 0)         # 1.25x
button.set_style_transform_pivot_x(50, 0)
button.set_style_transform_pivot_y(20, 0)
button.set_style_opa(lv.OPA._50, 0)
button.align(lv.ALIGN.CENTER, 0, 70)

label = lv.label(button)
label.set_text("Transf.")
label.center()

