#!/opt/bin/lv_micropython -i
import usys as sys
import lvgl as lv
import display_driver
from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image from the png file
try:
    with open('../../assets/img_cogwheel_argb.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find img_cogwheel_argb.png")
    sys.exit()

img_cogwheel_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

def create_slider(color):
    slider = lv.slider(lv.scr_act())
    slider.set_range(0, 255)
    slider.set_size(10, 200)
    slider.set_style_bg_color(color, lv.PART.KNOB)
    slider.set_style_bg_color(color.color_darken(lv.OPA._40), lv.PART.INDICATOR)
    slider.add_event_cb(slider_event_cb, lv.EVENT.VALUE_CHANGED, None)
    return slider

def slider_event_cb(e):
    # Recolor the image based on the sliders' values
    color  = lv.color_make(red_slider.get_value(), green_slider.get_value(), blue_slider.get_value())
    intense = intense_slider.get_value()
    img1.set_style_img_recolor_opa(intense, 0)
    img1.set_style_img_recolor(color, 0)

#
# Demonstrate runtime image re-coloring
#
# Create 4 sliders to adjust RGB color and re-color intensity
red_slider = create_slider(lv.palette_main(lv.PALETTE.RED))
green_slider = create_slider(lv.palette_main(lv.PALETTE.GREEN))
blue_slider = create_slider(lv.palette_main(lv.PALETTE.BLUE))
intense_slider = create_slider(lv.palette_main(lv.PALETTE.GREY))

red_slider.set_value(lv.OPA._20, lv.ANIM.OFF)
green_slider.set_value(lv.OPA._90, lv.ANIM.OFF)
blue_slider.set_value(lv.OPA._60, lv.ANIM.OFF)
intense_slider.set_value(lv.OPA._50, lv.ANIM.OFF)

red_slider.align(lv.ALIGN.LEFT_MID, 25, 0)
green_slider.align_to(red_slider, lv.ALIGN.OUT_RIGHT_MID, 25, 0)
blue_slider.align_to(green_slider, lv.ALIGN.OUT_RIGHT_MID, 25, 0)
intense_slider.align_to(blue_slider, lv.ALIGN.OUT_RIGHT_MID, 25, 0)

# Now create the actual image
img1 = lv.img(lv.scr_act())
img1.set_src(img_cogwheel_argb)
img1.align(lv.ALIGN.RIGHT_MID, -20, 0)

lv.event_send(intense_slider, lv.EVENT.VALUE_CHANGED, None)





