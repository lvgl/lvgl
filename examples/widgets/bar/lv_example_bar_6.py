def set_value(bar, v):
    bar.set_value(v, lv.ANIM.OFF)

def event_cb(e):
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.part != lv.PART.INDICATOR:
        return

    obj= e.get_target()

    label_dsc = lv.draw_label_dsc_t()
    label_dsc.init()
    # label_dsc.font = LV_FONT_DEFAULT;

    value_txt = str(obj.get_value())
    txt_size = lv.point_t()
    lv.txt_get_size(txt_size, value_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, lv.COORD.MAX, label_dsc.flag)

    txt_area = lv.area_t()
    # If the indicator is long enough put the text inside on the right
    if dsc.draw_area.get_width() > txt_size.x + 20:
        txt_area.x2 = dsc.draw_area.x2 - 5
        txt_area.x1 = txt_area.x2 - txt_size.x + 1
        label_dsc.color = lv.color_white()
    # If the indicator is still short put the text out of it on the right*/
    else:
        txt_area.x1 = dsc.draw_area.x2 + 5
        txt_area.x2 = txt_area.x1 + txt_size.x - 1
        label_dsc.color = lv.color_black()

    txt_area.y1 = dsc.draw_area.y1 + (dsc.draw_area.get_height() - txt_size.y) // 2
    txt_area.y2 = txt_area.y1 + txt_size.y - 1

    dsc.draw_ctx.label(label_dsc, txt_area, value_txt, None)

#
# Custom drawer on the bar to display the current value
#

bar = lv.bar(lv.scr_act())
bar.add_event_cb(event_cb, lv.EVENT.DRAW_PART_END, None)
bar.set_size(200, 20)
bar.center()

a = lv.anim_t()
a.init()
a.set_var(bar)
a.set_values(0, 100)
a.set_custom_exec_cb(lambda a,val: set_value(bar,val))
a.set_time(2000)
a.set_playback_time(2000)
a.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
lv.anim_t.start(a)

