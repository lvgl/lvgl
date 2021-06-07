#
# Demonstrate track placement
#

col_dsc = [60, 60, 60, lv.GRID_TEMPLATE.LAST]
row_dsc = [40, 40, 40, lv.GRID_TEMPLATE.LAST]


# Add space between the columns and move the rows to the bottom (end)

# Create a container with grid
cont = lv.obj(lv.scr_act())
cont.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.END)
cont.set_grid_dsc_array(col_dsc, row_dsc)
cont.set_size(300, 220)
cont.center()


for i in range(9):
    col = i % 3
    row = i // 3

    obj = lv.obj(cont)
    # Stretch the cell horizontally and vertically too
    # Set span to 1 to make the cell 1 column/row sized
    obj.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1,
                      lv.GRID_ALIGN.STRETCH, row, 1)

    label = lv.label(obj)
    label.set_text("{:d}{:d}".format(col, row))
    label.center()

