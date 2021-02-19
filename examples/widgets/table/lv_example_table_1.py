# Create a normal cell style
style_cell1 = lv.style_t()
lv.style_copy(style_cell1, lv.style_plain)
style_cell1.body.border.width = 1
style_cell1.body.border.color = lv.color_make(0,0,0)

# Crealte a header cell style
style_cell2 = lv.style_t()
lv.style_copy(style_cell2, lv.style_plain)
style_cell2.body.border.width = 1
style_cell2.body.border.color = lv.color_make(0,0,0)
style_cell2.body.main_color = lv.color_make(0xC0, 0xC0, 0xC0)
style_cell2.body.grad_color = lv.color_make(0xC0, 0xC0, 0xC0)

table = lv.table(lv.scr_act())
table.set_style(lv.table.STYLE.CELL1, style_cell1)
table.set_style(lv.table.STYLE.CELL2, style_cell2)
table.set_style(lv.table.STYLE.BG, lv.style_transp_tight)
table.set_col_cnt(2)
table.set_row_cnt(4)
table.align(None, lv.ALIGN.CENTER, 0, 0)

# Make the cells of the first row center aligned
table.set_cell_align(0, 0, lv.label.ALIGN.CENTER)
table.set_cell_align(0, 1, lv.label.ALIGN.CENTER)

# Make the cells of the first row TYPE = 2 (use `style_cell2`)
table.set_cell_type(0, 0, 2)
table.set_cell_type(0, 1, 2)

# Fill the first column
table.set_cell_value(0, 0, "Name")
table.set_cell_value(1, 0, "Apple")
table.set_cell_value(2, 0, "Banana")
table.set_cell_value(3, 0, "Citron")

# Fill the second column
table.set_cell_value(0, 1, "Price")
table.set_cell_value(1, 1, "$7")
table.set_cell_value(2, 1, "$4")
table.set_cell_value(3, 1, "$6")