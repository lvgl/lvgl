def sw_event_cb(e,panel):

    code = e.get_code()
    sw = e.get_target()

    if code == lv.EVENT.VALUE_CHANGED:

        if sw.has_state(lv.STATE.CHECKED):
            panel.add_flag(lv.obj.FLAG.SCROLL_ONE)
        else:
            panel.clear_flag(lv.obj.FLAG.SCROLL_ONE)


#
# Show an example to scroll snap
#

panel = lv.obj(lv.scr_act())
panel.set_size(280, 150)
panel.set_scroll_snap_x(lv.SCROLL_SNAP.CENTER)
panel.set_flex_flow(lv.FLEX_FLOW.ROW)
panel.center()

for i in range(10): 
    btn = lv.btn(panel)
    btn.set_size(150, 100)

    label = lv.label(btn)
    if i == 3:
        label.set_text("Panel {:d}\nno snap".format(i))
        btn.clear_flag(lv.obj.FLAG.SNAPPABLE)
    else:
        label.set_text("Panel {:d}".format(i))
    label.center()

panel.update_snap(lv.ANIM.ON)


# Switch between "One scroll" and "Normal scroll" mode
sw = lv.switch(lv.scr_act());
sw.align(lv.ALIGN.TOP_RIGHT, -20, 10)
sw.add_event_cb(lambda evt:  sw_event_cb(evt,panel), lv.EVENT.ALL, None)
label = lv.label(lv.scr_act())
label.set_text("One scroll")
label.align_to(sw, lv.ALIGN.OUT_BOTTOM_MID, 0, 5)


