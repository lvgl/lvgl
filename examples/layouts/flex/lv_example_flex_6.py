#
# RTL base direction changes order of the items.
# Also demonstrate how horizontal scrolling works with RTL.
#

cont = lv.obj(lv.scr_act())
cont.set_style_base_dir(lv.BASE_DIR.RTL,0)
cont.set_size(300, 220)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

for i in range(20):
    obj = lv.obj(cont)
    obj.set_size(70, lv.SIZE.CONTENT)

    label = lv.label(obj)
    label.set_text(str(i))
    label.center()

