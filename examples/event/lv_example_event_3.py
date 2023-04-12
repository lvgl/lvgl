def event_cb(e):

    # The original target of the event. Can be the buttons or the container
    target = e.get_target_obj()

    # The current target is always the container as the event is added to it
    cont = e.get_current_target_obj()

    # If container was clicked do nothing
    if target == cont:
        return

    # Make the clicked buttons red
    target.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), 0)

#
# Demonstrate event bubbling
#

cont = lv.obj(lv.scr_act())
cont.set_size(290, 200)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

for i in range(30):
    btn = lv.btn(cont)
    btn.set_size(70, 50)
    btn.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    label = lv.label(btn)
    label.set_text("{:d}".format(i))
    label.center()

cont.add_event(event_cb, lv.EVENT.CLICKED, None)
