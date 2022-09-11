def event_cb(e):

    # The original target of the event. Can be the buttons or the container
    target = e.get_target()
    # print(type(target))

    # If container was clicked do nothing
    if type(target) != type(lv.btn()):
        return

    # Make the clicked buttons red
    target.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), 0)

#
# Demonstrate event bubbling
#

cont = lv.obj(lv.scr_act())
cont.set_size(320, 200)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

for i in range(30):
    btn = lv.btn(cont)
    btn.set_size(80, 50)
    btn.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    label = lv.label(btn)
    label.set_text(str(i))
    label.center()

cont.add_event_cb(event_cb, lv.EVENT.CLICKED, None)
