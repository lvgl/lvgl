#
# Grid navigation on a list
#
# It's assumed that the default group is set and
# there is a keyboard indev

list1 = lv.list(lv.scr_act())
lv.gridnav_add(list1, lv.GRIDNAV_CTRL.NONE)
list1.set_size(lv.pct(45), lv.pct(80))
list1.align(lv.ALIGN.LEFT_MID, 5, 0)
list1.set_style_bg_color(lv.palette_lighten(lv.PALETTE.BLUE, 5), lv.STATE.FOCUSED)
lv.group_get_default().add_obj(list1)

for i in range(15):
    item_text = "File {:d}".format(i)
    item = list1.add_btn(lv.SYMBOL.FILE, item_text)
    item.set_style_bg_opa(0, 0)
    lv.group_remove_obj(item)   # Not needed, we use the gridnav instead

list2 = lv.list(lv.scr_act())
lv.gridnav_add(list2, lv.GRIDNAV_CTRL.ROLLOVER)
list2.set_size(lv.pct(45), lv.pct(80))
list2.align(lv.ALIGN.RIGHT_MID, -5, 0)
list2.set_style_bg_color(lv.palette_lighten(lv.PALETTE.BLUE, 5), lv.STATE.FOCUSED)
lv.group_get_default().add_obj(list2)

for i in range(15):
    item_text = "Folder {:d}".format(i)
    item = list2.add_btn(lv.SYMBOL.DIRECTORY, item_text)
    item.set_style_bg_opa(0, 0)
    lv.group_remove_obj(item)

