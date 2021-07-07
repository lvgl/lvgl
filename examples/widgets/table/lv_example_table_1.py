def draw_part_event_cb(e):
    obj = e.get_target()
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    # If the cells are drawn../
    if dsc.part == lv.PART.ITEMS:
        row = dsc.id //  obj.get_col_cnt()
        col = dsc.id - row * obj.get_col_cnt()

        # Make the texts in the first cell center aligned
        if row == 0:
            dsc.label_dsc.align = lv.TEXT_ALIGN.CENTER
            dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE).color_mix(dsc.rect_dsc.bg_color, lv.OPA._20)
            dsc.rect_dsc.bg_opa = lv.OPA.COVER

        # In the first column align the texts to the right
        elif col == 0:
            dsc.label_dsc.flag = lv.TEXT_ALIGN.RIGHT

        # Make every 2nd row grayish
        if row != 0 and (row % 2) == 0:
            dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.GREY).color_mix(dsc.rect_dsc.bg_color, lv.OPA._10)
            dsc.rect_dsc.bg_opa = lv.OPA.COVER


table = lv.table(lv.scr_act())

# Fill the first column
table.set_cell_value(0, 0, "Name");
table.set_cell_value(1, 0, "Apple");
table.set_cell_value(2, 0, "Banana");
table.set_cell_value(3, 0, "Lemon");
table.set_cell_value(4, 0, "Grape");
table.set_cell_value(5, 0, "Melon");
table.set_cell_value(6, 0, "Peach");
table.set_cell_value(7, 0, "Nuts");

# Fill the second column
table.set_cell_value(0, 1, "Price");
table.set_cell_value(1, 1, "$7");
table.set_cell_value(2, 1, "$4");
table.set_cell_value(3, 1, "$6");
table.set_cell_value(4, 1, "$2");
table.set_cell_value(5, 1, "$5");
table.set_cell_value(6, 1, "$1");
table.set_cell_value(7, 1, "$9");

# Set a smaller height to the table. It'll make it scrollable
table.set_height(200)
table.center()

# Add an event callback to to apply some custom drawing
table.add_event_cb(draw_part_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

