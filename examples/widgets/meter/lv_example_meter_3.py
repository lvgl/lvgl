#!//opt/bin/lv_micropython -i
import utime as time
import lvgl as lv
import display_driver
from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image from the png file
try:
    with open('../../assets/img_hand_min.png','rb') as f:
        img_hand_min_data = f.read()
except:
    print("Could not find img_hand_min.png")
    sys.exit()
    
img_hand_min_dsc = lv.img_dsc_t({
  'data_size': len(img_hand_min_data),
  'data': img_hand_min_data 
})

# Create an image from the png file
try:
    with open('../../assets/img_hand_hour.png','rb') as f:
        img_hand_hour_data = f.read()
except:
    print("Could not find img_hand_hour.png")
    sys.exit()
    
img_hand_hour_dsc = lv.img_dsc_t({
  'data_size': len(img_hand_hour_data),
  'data': img_hand_hour_data 
})

def set_value(indic, v):
    meter.set_indicator_value(indic, v)
#
# A clock from a meter
#

meter = lv.meter(lv.scr_act())
meter.set_size(220, 220)
meter.center()

# Create a scale for the minutes
# 61 ticks in a 360 degrees range (the last and the first line overlaps)
scale_min = meter.add_scale()
meter.set_scale_ticks(scale_min, 61, 1, 10, lv.palette_main(lv.PALETTE.GREY))
meter.set_scale_range(scale_min, 0, 60, 360, 270)

# Create another scale for the hours. It's only visual and contains only major ticks
scale_hour = meter.add_scale()
meter.set_scale_ticks(scale_hour, 12, 0, 0, lv.palette_main(lv.PALETTE.GREY))  # 12 ticks
meter.set_scale_major_ticks(scale_hour, 1, 2, 20, lv.color_black(), 10)         # Every tick is major
meter.set_scale_range(scale_hour, 1, 12, 330, 300)                             # [1..12] values in an almost full circle

#    LV_IMG_DECLARE(img_hand)

# Add the hands from images
indic_min = meter.add_needle_img(scale_min, img_hand_min_dsc, 5, 5)
indic_hour = meter.add_needle_img(scale_min, img_hand_hour_dsc, 5, 5)

# Create an animation to set the value
a1 = lv.anim_t()
a1.init()
a1.set_values(0, 60)
a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
a1.set_time(2000)        # 2 sec for 1 turn of the minute hand (1 hour)
a1.set_var(indic_min)
a1.set_custom_exec_cb(lambda a1,val: set_value(indic_min,val))
lv.anim_t.start(a1)

a2 = lv.anim_t()
a2.init()
a2.set_var(indic_hour)
a2.set_time(24000)       # 24 sec for 1 turn of the hour hand
a2.set_values(0, 60)
a2.set_custom_exec_cb(lambda a2,val: set_value(indic_hour,val))            
lv.anim_t.start(a2)

