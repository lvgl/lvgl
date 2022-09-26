def cont_sub_event_cb(e):
    k = e.get_key()
    obj = e.get_current_target()
    if k == lv.KEY.ENTER:
        lv.group_focus_obj(obj)

    elif k == lv.KEY.ESC:
        obj.get_group().focus_next()

#
# Nested grid navigations
#
# It's assumed that the default group is set and
# there is a keyboard indev*/

cont_main = lv.obj(lv.scr_act())
lv.gridnav_add(cont_main,lv.GRIDNAV_CTRL.ROLLOVER | lv.GRIDNAV_CTRL.SCROLL_FIRST)

# Only the container needs to be in a group
lv.group_get_default().add_obj(cont_main)

# Use flex here, but works with grid or manually placed objects as well
cont_main.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
cont_main.set_style_bg_color(lv.palette_lighten(lv.PALETTE.BLUE, 5), lv.STATE.FOCUSED)
cont_main.set_size(lv.pct(80), lv.SIZE_CONTENT)

btn = lv.btn(cont_main)
lv.group_remove_obj(btn)
label = lv.label(btn)
label.set_text("Button 1")

btn = lv.btn(cont_main)
lv.group_remove_obj(btn)
label = lv.label(btn)
label.set_text("Button 2");


# Create an other container with long text to show how LV_GRIDNAV_CTRL_SCROLL_FIRST works
cont_sub1 = lv.obj(cont_main)
cont_sub1.set_size(lv.pct(100), 100)

label = lv.label(cont_sub1)
cont_sub1.set_style_bg_color(lv.palette_lighten(lv.PALETTE.RED, 5), lv.STATE.FOCUSED)
label.set_width(lv.pct(100));
label.set_text(
    """I'm a very long text which makes my container scrollable. 
    As LV_GRIDNAV_FLAG_SCROLL_FIRST is enabled arrows will scroll me first 
    and a new objects will be focused only when an edge is reached with the scrolling.\n
    This is only some placeholder text to be sure the parent will be scrollable. \n
    Hello world!
    Hello world!
    Hello world!
    Hello world!
    Hello world!
    Hello world!
    """)

# Create a third container that can be focused with ENTER and contains an other grid nav
cont_sub2 = lv.obj(cont_main)
lv.gridnav_add(cont_sub2, lv.GRIDNAV_CTRL.ROLLOVER)
#Only the container needs to be in a group
lv.group_get_default().add_obj(cont_sub2)

cont_sub2.add_event_cb(cont_sub_event_cb, lv.EVENT.KEY, None)

# Use flex here, but works with grid or manually placed objects as well
cont_sub2.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
cont_sub2.set_style_bg_color(lv.palette_lighten(lv.PALETTE.RED, 5), lv.STATE.FOCUSED)
cont_sub2.set_size(lv.pct(100), lv.SIZE_CONTENT)

label = lv.label(cont_sub2)
label.set_text("Use ENTER/ESC to focus/defocus this container")
label.set_width(lv.pct(100))

btn = lv.btn(cont_sub2)
lv.group_remove_obj(btn)
label = lv.label(btn)
label.set_text("Button 3")

btn = lv.btn(cont_sub2)
lv.group_remove_obj(btn)
label = lv.label(btn)
label.set_text("Button 4")

