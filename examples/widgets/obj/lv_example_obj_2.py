def drag_event_handler(e):

    obj = e.get_target_obj()

    indev = lv.indev_get_act()

    vect = lv.point_t()
    indev.get_vect(vect)
    x = obj.get_x() + vect.x
    y = obj.get_y() + vect.y
    obj.set_pos(x, y)


#
# Make an object draggable.
#

obj = lv.obj(lv.screen_active())
obj.set_size(150, 100)
obj.add_event(drag_event_handler, lv.EVENT.PRESSING, None)

label = lv.label(obj)
label.set_text("Drag me")
label.center()

