def anim_x_cb(label, v):
    label.set_x(v)

def sw_event_cb(e,label):
    sw = e.get_target()

    if sw.has_state(lv.STATE.CHECKED):
        a = lv.anim_t()
        a.init()
        a.set_var(label)
        a.set_values(label.get_x(), 100)
        a.set_time(500)
        a.set_path_cb(lv.anim_t.path_overshoot)
        a.set_custom_exec_cb(lambda a,val: anim_x_cb(label,val))
        lv.anim_t.start(a)
    else:
        a = lv.anim_t()
        a.init()
        a.set_var(label)
        a.set_values(label.get_x(), -label.get_width())
        a.set_time(500)
        a.set_path_cb(lv.anim_t.path_ease_in)
        a.set_custom_exec_cb(lambda a,val: anim_x_cb(label,val))
        lv.anim_t.start(a)

#
# Start animation on an event
#

label = lv.label(lv.scr_act())
label.set_text("Hello animations!")
label.set_pos(100, 10)


sw = lv.switch(lv.scr_act())
sw.center()
sw.add_state(lv.STATE.CHECKED)
sw.add_event_cb(lambda e: sw_event_cb(e,label), lv.EVENT.VALUE_CHANGED, None)



