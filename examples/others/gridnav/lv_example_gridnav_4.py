def event_handler(e):
    obj = e.get_target_obj()
    list = obj.get_parent()
    print("Clicked: " + list.get_btn_text(obj))


#
# Simple navigation on a list widget
#
# It's assumed that the default group is set and
# there is a keyboard indev

list = lv.list(lv.scr_act())
lv.gridnav_add(list, lv.GRIDNAV_CTRL.ROLLOVER)
list.align(lv.ALIGN.LEFT_MID, 0, 10)
lv.group_get_default().add_obj(list)

for i in range(20):

    # Add some separators too, they are not focusable by gridnav
    if i % 5 == 0:
        txt = "Section {:d}".format(i // 5 + 1)
        # lv_snprintf(buf, sizeof(buf), "Section %d", i / 5 + 1);
        list.add_text(txt)

    txt = "File {:d}".format(i + 1)
    #lv_snprintf(buf, sizeof(buf), "File %d", i + 1);
    item = list.add_btn(lv.SYMBOL.FILE, txt)
    item.add_event(event_handler, lv.EVENT.CLICKED, None)
    lv.group_remove_obj(item)  # The default group adds it automatically

btn = lv.btn(lv.scr_act())
btn.align(lv.ALIGN.RIGHT_MID, 0, -10)
label = lv.label(btn)
label.set_text("Button")

