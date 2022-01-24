def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_size_cb(obj, v):
    obj.set_size(v, v)


#
# Create a playback animation
#
obj = lv.obj(lv.scr_act())
obj.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), 0)
obj.set_style_radius(lv.RADIUS.CIRCLE, 0)

obj.align(lv.ALIGN.LEFT_MID, 10, 0)

a1 = lv.anim_t()
a1.init()
a1.set_var(obj)
a1.set_values(10, 50)
a1.set_time(1000)
a1.set_playback_delay(100)
a1.set_playback_time(300)
a1.set_repeat_delay(500)
a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
a1.set_path_cb(lv.anim_t.path_ease_in_out)
a1.set_custom_exec_cb(lambda a1,val: anim_size_cb(obj,val))
lv.anim_t.start(a1)

a2 = lv.anim_t()
a2.init()
a2.set_var(obj)
a2.set_values(10, 240)
a2.set_time(1000)
a2.set_playback_delay(100)
a2.set_playback_time(300)
a2.set_repeat_delay(500)
a2.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
a2.set_path_cb(lv.anim_t.path_ease_in_out)
a2.set_custom_exec_cb(lambda a1,val: anim_x_cb(obj,val))
lv.anim_t.start(a2)
