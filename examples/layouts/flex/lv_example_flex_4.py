#
# Reverse the order of flex items
#
cont = lv.obj(lv.scr_act())
cont.set_size(300, 220)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.COLUMN_REVERSE)

for i  in range(6):
    obj = lv.obj(cont)
    obj.set_size(100, 50)

    label = lv.label(obj)
    label.set_text("Item: " + str(i))
    label.center()

