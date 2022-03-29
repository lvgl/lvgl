def slider_event_cb(e):
    code = e.get_code()
    obj = e.get_target()

    # Provide some extra space for the value
    if code == lv.EVENT.REFR_EXT_DRAW_SIZE:
        e.set_ext_draw_size(50)

    elif code == lv.EVENT.DRAW_PART_END:
        # print("DRAW_PART_END")
        dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
        # print(dsc)
        if dsc.part == lv.PART.INDICATOR:
            label_text = "{:d} - {:d}".format(obj.get_left_value(),slider.get_value())
            label_size = lv.point_t()
            lv.txt_get_size(label_size, label_text, lv.font_default(), 0, 0, lv.COORD.MAX, 0)
            # print(label_size.x,label_size.y)
            label_area = lv.area_t()
            label_area.x1 = dsc.draw_area.x1 + dsc.draw_area.get_width() // 2 - label_size.x // 2
            label_area.x2 = label_area.x1 + label_size.x
            label_area.y2 = dsc.draw_area.y1 - 10
            label_area.y1 = label_area.y2 - label_size.y

            label_draw_dsc = lv.draw_label_dsc_t()
            label_draw_dsc.init()

            dsc.draw_ctx.label(label_draw_dsc, label_area, label_text, None)
#
# Show the current value when the slider if pressed by extending the drawer
#
#
#Create a slider in the center of the display

slider = lv.slider(lv.scr_act())
slider.center()

slider.set_mode(lv.slider.MODE.RANGE)
slider.set_value(70, lv.ANIM.OFF)
slider.set_left_value(20, lv.ANIM.OFF)

slider.add_event_cb(slider_event_cb, lv.EVENT.ALL, None)
slider.refresh_ext_draw_size()

