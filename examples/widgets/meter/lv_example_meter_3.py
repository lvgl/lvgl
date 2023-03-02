import utime as time

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

def tick_label_event(e):
    draw_part_dsc = e.get_draw_part_dsc();

    # Be sure it's drawing the ticks
    if draw_part_dsc.type != lv.meter.DRAW_PART.TICK: return

    # Be sure it's a major ticks
    if draw_part_dsc.id % 5:  return

    # The order of numbers on the clock is tricky: 12, 1, 2, 3...*/
    txt = ["12", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"]
    # dsc.text is defined char text[16], I must therefore convert the Python string to a byte_array

    idx = int(draw_part_dsc.id / 5)
    draw_part_dsc.text = bytes(txt[idx],"ascii")

meter = lv.meter(lv.scr_act())
meter.set_size(220, 220)
meter.center()

# Create a scale for the minutes
# 60 ticks in a 354 degrees range
meter.set_scale_ticks(60, 1, 10, lv.palette_main(lv.PALETTE.GREY))
meter.set_scale_major_ticks(5, 2, 20, lv.color_black(), 10)         # Every tick is major
meter.set_scale_range(0, 59, 354, 270)

# Add the hands from images
indic_min = meter.add_needle_img(img_hand_min_dsc, 5, 5)
indic_hour = meter.add_needle_img(img_hand_hour_dsc, 5, 5)

#Add an event to set the numbers of hours
meter.add_event(tick_label_event, lv.EVENT.DRAW_PART_BEGIN, None)

# Create an animation to set the value
a1 = lv.anim_t()
a1.init()
a1.set_values(0, 60)
a1.set_repeat_count(lv.ANIM_REPEAT_INFINITE)
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

