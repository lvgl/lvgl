#
# Create a fake text shadow
#

# Create a style for the shadow
style_shadow = lv.style_t()
style_shadow.init()
style_shadow.set_text_opa(lv.OPA._30)
style_shadow.set_text_color(lv.color_black())

# Create a label for the shadow first (it's in the background)
shadow_label = lv.label(lv.scr_act())
shadow_label.add_style(style_shadow, 0)

# Create the main label
main_label = lv.label(lv.scr_act())
main_label.set_text("A simple method to create\n"
                   "shadows on a text.\n"
                   "It even works with\n\n"
                   "newlines     and spaces.")

# Set the same text for the shadow label
shadow_label.set_text(lv.label.get_text(main_label))

# Position the main label
main_label.align(lv.ALIGN.CENTER, 0, 0)

# Shift the second label down and to the right by 2 pixel
shadow_label.align_to(main_label, lv.ALIGN.TOP_LEFT, 2, 2)

