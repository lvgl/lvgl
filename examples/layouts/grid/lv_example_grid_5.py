def row_gap_anim(obj, v):
    obj.set_style_pad_row(v, 0)

def column_gap_anim(obj, v):
    obj.set_style_pad_column(v, 0)

#
# Demonstrate column and row gap
#

# 60x60 cells
col_dsc = [60, 60, 60, lv.GRID_TEMPLATE.LAST]
row_dsc = [40, 40, 40, lv.GRID_TEMPLATE.LAST]

# Create a container with grid
cont = lv.obj(lv.scr_act())
cont.set_size(300, 220)
cont.center()
cont.set_grid_dsc_array(col_dsc, row_dsc)

for i in range(9):
    col = i % 3
    row = i // 3

    obj = lv.obj(cont)
    obj.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1,
                      lv.GRID_ALIGN.STRETCH, row, 1)
    label = lv.label(obj)
    label.set_text("{:d},{:d}".format(col, row))
    label.center()

    a_row = lv.anim_t()
    a_row.init()
    a_row.set_var(cont)
    a_row.set_values(0, 10)
    a_row.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    a_row.set_time(500)
    a_row.set_playback_time(500)
    a_row. set_custom_exec_cb(lambda a,val: row_gap_anim(cont,val))
    lv.anim_t.start(a_row)

    a_col = lv.anim_t()
    a_col.init()
    a_col.set_var(cont)
    a_col.set_values(0, 10)
    a_col.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    a_col.set_time(500)
    a_col.set_playback_time(500)
    a_col. set_custom_exec_cb(lambda a,val: column_gap_anim(cont,val))
    lv.anim_t.start(a_col)


