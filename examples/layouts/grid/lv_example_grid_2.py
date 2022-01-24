#
# Demonstrate cell placement and span
#

col_dsc = [70, 70, 70, lv.GRID_TEMPLATE.LAST]
row_dsc = [50, 50, 50, lv.GRID_TEMPLATE.LAST]

# Create a container with grid
cont = lv.obj(lv.scr_act())
cont.set_grid_dsc_array(col_dsc, row_dsc)
cont.set_size(300, 220)
cont.center()

# Cell to 0;0 and align to to the start (left/top) horizontally and vertically too
obj = lv.obj(cont)
obj.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
obj.set_grid_cell(lv.GRID_ALIGN.START, 0, 1,
                  lv.GRID_ALIGN.START, 0, 1)
label = lv.label(obj);
label.set_text("c0, r0")

# Cell to 1;0 and align to to the start (left) horizontally and center vertically too
obj = lv.obj(cont)
obj.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
obj.set_grid_cell(lv.GRID_ALIGN.START, 1, 1,
                  lv.GRID_ALIGN.CENTER, 0, 1)
label = lv.label(obj)
label.set_text("c1, r0")

# Cell to 2;0 and align to to the start (left) horizontally and end (bottom) vertically too
obj = lv.obj(cont)
obj.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
obj.set_grid_cell(lv.GRID_ALIGN.START, 2, 1,
                  lv.GRID_ALIGN.END, 0, 1)
label = lv.label(obj)
label.set_text("c2, r0");

# Cell to 1;1 but 2 column wide (span = 2).Set width and height to stretched.
obj = lv.obj(cont)
obj.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
obj.set_grid_cell(lv.GRID_ALIGN.STRETCH, 1, 2,
                  lv.GRID_ALIGN.STRETCH, 1, 1)
label = lv.label(obj)
label.set_text("c1-2, r1")

# Cell to 0;1 but 2 rows tall (span = 2).Set width and height to stretched.
obj = lv.obj(cont)
obj.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
obj.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1,
                  lv.GRID_ALIGN.STRETCH, 1, 2)
label = lv.label(obj)
label.set_text("c0\nr1-2")

