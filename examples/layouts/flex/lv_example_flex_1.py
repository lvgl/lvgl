#
# A simple row and a column layout with flexbox
#

# Create a container with ROW flex direction
cont_row = lv.obj(lv.scr_act())
cont_row.set_size(300, 75)
cont_row.align(lv.ALIGN.TOP_MID, 0, 5)
cont_row.set_flex_flow(lv.FLEX_FLOW.ROW)

# Create a container with COLUMN flex direction
cont_col = lv.obj(lv.scr_act())
cont_col.set_size(200, 150)
cont_col.align_to(cont_row, lv.ALIGN.OUT_BOTTOM_MID, 0, 5)
cont_col.set_flex_flow(lv.FLEX_FLOW.COLUMN)

for i in range(10):
    # Add items to the row
    obj = lv.btn(cont_row)
    obj.set_size(100, lv.pct(100))

    label = lv.label(obj)
    label.set_text("Item: {:d}".format(i))
    label.center()

    # Add items to the column
    obj = lv.btn(cont_col)
    obj.set_size(lv.pct(100), lv.SIZE.CONTENT)

    label = lv.label(obj)
    label.set_text("Item: {:d}".format(i))
    label.center()

