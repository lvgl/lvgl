#
# Opacity and Transformations
#

# Normal button
btn = lv.btn(lv.scr_act())
btn.set_size(100, 40)
btn.align(lv.ALIGN.CENTER, 0, -70)

label = lv.label(btn)
label.set_text("Normal")
label.center()

# Set opacity
# The button and the label is rendered to a layer first and that layer is blended
btn = lv.btn(lv.scr_act())
btn.set_size(100, 40)
btn.set_style_opa(lv.OPA._50, 0)
btn.align(lv.ALIGN.CENTER, 0, 0)

label = lv.label(btn)
label.set_text("Opa:50%")
label.center()

# Set transformations
# The button and the label is rendered to a layer first and that layer is transformed
btn = lv.btn(lv.scr_act())
btn.set_size(100, 40)
btn.set_style_transform_angle(150, 0)             # 15 deg
btn.set_style_transform_zoom(256 + 64, 0)         # 1.25x
btn.set_style_transform_pivot_x(50, 0)
btn.set_style_transform_pivot_y(20, 0)
btn.set_style_opa(lv.OPA._50, 0)
btn.align(lv.ALIGN.CENTER, 0, 70)

label = lv.label(btn)
label.set_text("Transf.")
label.center()

