#
# Arrange items in rows with wrap and place the items to get even space around them.
#
style = lv.style_t()
style.init()
style.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
style.set_flex_main_place(lv.FLEX_ALIGN.SPACE_EVENLY)
style.set_layout(lv.LAYOUT.FLEX)

cont = lv.obj(lv.screen_active())
cont.set_size(300, 220)
cont.center()
cont.add_style(style, 0)

for i in range(8):
    obj = lv.obj(cont)
    obj.set_size(70, lv.SIZE_CONTENT)

    label = lv.label(obj)
    label.set_text("{:d}".format(i))
    label.center()

