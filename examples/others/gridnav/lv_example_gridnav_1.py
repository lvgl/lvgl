#
# Demonstrate a a basic grid navigation
#
# It's assumed that the default group is set and
# there is a keyboard indev

cont1 = lv.obj(lv.scr_act())
lv.gridnav_add(cont1, lv.GRIDNAV_CTRL.NONE)

# Use flex here, but works with grid or manually placed objects as well
cont1.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
cont1.set_style_bg_color(lv.palette_lighten(lv.PALETTE.BLUE, 5), lv.STATE.FOCUSED)
cont1.set_size(lv.pct(50), lv.pct(100))

# Only the container needs to be in a group
lv.group_get_default().add_obj(cont1)

label = lv.label(cont1)
label.set_text("No rollover")

for i in range(10):
    obj = lv.btn(cont1)
    obj.set_size(70, lv.SIZE_CONTENT)
    obj.add_flag(lv.obj.FLAG.CHECKABLE)
    lv.group_remove_obj(obj)   # Not needed, we use the gridnav instead

    label = lv.label(obj)
    label.set_text("{:d}".format(i))
    label.center()

# Create a second container with rollover grid nav mode.

cont2 = lv.obj(lv.scr_act())
lv.gridnav_add(cont2,lv.GRIDNAV_CTRL.ROLLOVER)
cont2.set_style_bg_color(lv.palette_lighten(lv.PALETTE.BLUE, 5), lv.STATE.FOCUSED)
cont2.set_size(lv.pct(50), lv.pct(100))
cont2.align(lv.ALIGN.RIGHT_MID, 0, 0)

label = lv.label(cont2)
label.set_width(lv.pct(100))
label.set_text("Rollover\nUse tab to focus the other container")

# Only the container needs to be in a group
lv.group_get_default().add_obj(cont2)

# Add and place some children manually
ta = lv.textarea(cont2)
ta.set_size(lv.pct(100), 80)
ta.set_pos(0, 80);
lv.group_remove_obj(ta)   # Not needed, we use the gridnav instead

cb = lv.checkbox(cont2)
cb.set_pos(0, 170)
lv.group_remove_obj(cb)   # Not needed, we use the gridnav instead

sw1 = lv.switch(cont2)
sw1.set_pos(0, 200);
lv.group_remove_obj(sw1)  # Not needed, we use the gridnav instead

sw2 = lv.switch(cont2)
sw2.set_pos(lv.pct(50), 200)
lv.group_remove_obj(sw2)  # Not needed, we use the gridnav instead

