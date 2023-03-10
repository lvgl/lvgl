def value_changed_event_cb(e,label):
    arc = e.get_target_obj()

    txt = "{:d}%".format(arc.get_value())
    label.set_text(txt)

    # Rotate the label to the current position of the arc
    arc.rotate_obj_to_angle(label, 25)

label = lv.label(lv.scr_act())

# Create an Arc
arc = lv.arc(lv.scr_act())
arc.set_size(150, 150)
arc.set_rotation(135)
arc.set_bg_angles(0, 270)
arc.set_value(10)
arc.center()
arc.add_event(lambda e: value_changed_event_cb(e,label),lv.EVENT.VALUE_CHANGED, None)

# Manually update the label for the first time
arc.send_event(lv.EVENT.VALUE_CHANGED, None)

