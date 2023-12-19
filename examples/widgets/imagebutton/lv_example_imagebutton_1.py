# Create an image from the png file
try:
    with open('../../assets/imgbtn_left.png','rb') as f:
        imagebutton_left_data = f.read()
except:
    print("Could not find imagebutton_left.png")
    sys.exit()

imagebutton_left_dsc = lv.image_dsc_t({
  'data_size': len(imagebutton_left_data),
  'data': imagebutton_left_data
})

try:
    with open('../../assets/imgbtn_mid.png','rb') as f:
        imagebutton_mid_data = f.read()
except:
    print("Could not find imagebutton_mid.png")
    sys.exit()

imagebutton_mid_dsc = lv.image_dsc_t({
  'data_size': len(imagebutton_mid_data),
  'data': imagebutton_mid_data
})

try:
    with open('../../assets/imgbtn_right.png','rb') as f:
        imagebutton_right_data = f.read()
except:
    print("Could not find imgtn_right.png")
    sys.exit()

imagebutton_right_dsc = lv.image_dsc_t({
  'data_size': len(imagebutton_right_data),
  'data': imagebutton_right_data
})

# Create a transition animation on width transformation and recolor.
tr_prop = [lv.STYLE.TRANSFORM_WIDTH, lv.STYLE.IMAGE_RECOLOR_OPA, 0]
tr = lv.style_transition_dsc_t()
tr.init(tr_prop, lv.anim_t.path_linear, 200, 0, None)

style_def = lv.style_t()
style_def.init()
style_def.set_text_color(lv.color_white())
style_def.set_transition(tr)

# Darken the button when pressed and make it wider
style_pr = lv.style_t()
style_pr.init()
style_pr.set_image_recolor_opa(lv.OPA._30)
style_pr.set_image_recolor(lv.color_black())
style_pr.set_transform_width(20)

# Create an image button
imagebutton1 = lv.imagebutton(lv.screen_active())
imagebutton1.set_src(lv.imagebutton.STATE.RELEASED, imagebutton_left_dsc, imagebutton_mid_dsc, imagebutton_right_dsc)
imagebutton1.add_style(style_def, 0)
imagebutton1.add_style(style_pr, lv.STATE.PRESSED)

imagebutton1.align(lv.ALIGN.CENTER, 0, 0)

# Create a label on the image button
label = lv.label(imagebutton1)
label.set_text("Button")
label.align(lv.ALIGN.CENTER, 0, -4)

