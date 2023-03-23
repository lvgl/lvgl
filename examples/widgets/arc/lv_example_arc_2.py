def set_angle(obj, v):
    obj.set_value(v)

#
# Create an arc which acts as a loader.
#
# Create an Arc
arc = lv.arc(lv.scr_act())
arc.set_rotation(270)
arc.set_bg_angles(0, 360)
arc.remove_style(None, lv.PART.KNOB)   # Be sure the knob is not displayed
arc.clear_flag(lv.obj.FLAG.CLICKABLE)  #To not allow adjusting by click
arc.center()

a = lv.anim_t()
a.init()
a.set_var(arc)
a.set_time(1000)
a.set_repeat_count(lv.ANIM_REPEAT_INFINITE)    #Just for the demo
a.set_repeat_delay(500)
a.set_values(0, 100)
a.set_custom_exec_cb(lambda a,val: set_angle(arc,val))
lv.anim_t.start(a)

