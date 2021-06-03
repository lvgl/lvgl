def row_gap_anim(obj, v):
    obj.set_style_pad_row(v, 0)


def column_gap_anim(obj, v):
    obj.set_style_pad_column(v, 0)

#
# Demonstrate the effect of column and row gap style properties
#

cont = lv.obj(lv.scr_act())
cont.set_size(300, 220)
cont.center()
cont.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)

for i in range(9):
    obj = lv.obj(cont)
    obj.set_size(70, lv.SIZE.CONTENT)

    label = lv.label(obj)
    label.set_text(str(i))
    label.center()

a_row = lv.anim_t()
a_row.init()
a_row.set_var(cont)
a_row.set_values(0, 10)
a_row.set_repeat_count(lv.ANIM_REPEAT.INFINITE)

a_row.set_time(500)
a_row.set_playback_time(500)
a_row.set_custom_exec_cb(lambda a,val: row_gap_anim(cont,val))
lv.anim_t.start(a_row)

a_col = lv.anim_t()
a_col.init()
a_col.set_var(cont)
a_col.set_values(0, 10)
a_col.set_repeat_count(lv.ANIM_REPEAT.INFINITE)

a_col.set_time(3000)
a_col.set_playback_time(3000)
a_col.set_custom_exec_cb(lambda a,val: column_gap_anim(cont,val))

lv.anim_t.start(a_col)

