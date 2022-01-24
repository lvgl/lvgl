from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image from the png file
try:
    with open('../../assets/imgbtn_left.png','rb') as f:
        imgbtn_left_data = f.read()
except:
    print("Could not find imgbtn_left.png")
    sys.exit()
    
imgbtn_left_dsc = lv.img_dsc_t({
  'data_size': len(imgbtn_left_data),
  'data': imgbtn_left_data 
})

try:
    with open('../../assets/imgbtn_mid.png','rb') as f:
        imgbtn_mid_data = f.read()
except:
    print("Could not find imgbtn_mid.png")
    sys.exit()
    
imgbtn_mid_dsc = lv.img_dsc_t({
  'data_size': len(imgbtn_mid_data),
  'data': imgbtn_mid_data 
})

try:
    with open('../../assets/imgbtn_right.png','rb') as f:
        imgbtn_right_data = f.read()
except:
    print("Could not find imgbtn_right.png")
    sys.exit()
    
imgbtn_right_dsc = lv.img_dsc_t({
  'data_size': len(imgbtn_right_data),
  'data': imgbtn_right_data 
})

# Create a transition animation on width transformation and recolor.
tr_prop = [lv.STYLE.TRANSFORM_WIDTH, lv.STYLE.IMG_RECOLOR_OPA, 0]
tr = lv.style_transition_dsc_t()
tr.init(tr_prop, lv.anim_t.path_linear, 200, 0, None)

style_def = lv.style_t()
style_def.init()
style_def.set_text_color(lv.color_white())
style_def.set_transition(tr)

# Darken the button when pressed and make it wider
style_pr = lv.style_t()
style_pr.init()
style_pr.set_img_recolor_opa(lv.OPA._30)
style_pr.set_img_recolor(lv.color_black())
style_pr.set_transform_width(20)

# Create an image button
imgbtn1 = lv.imgbtn(lv.scr_act())
imgbtn1.set_src(lv.imgbtn.STATE.RELEASED, imgbtn_left_dsc, imgbtn_mid_dsc, imgbtn_right_dsc)
imgbtn1.add_style(style_def, 0)
imgbtn1.add_style(style_pr, lv.STATE.PRESSED)

imgbtn1.align(lv.ALIGN.CENTER, 0, 0)

# Create a label on the image button
label = lv.label(imgbtn1)
label.set_text("Button");
label.align(lv.ALIGN.CENTER, 0, -4)

