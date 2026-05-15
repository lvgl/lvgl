/**
 * @file lv_example_table_merge_cells.c
 */

#include "../../../../lvgl.h"

/**
 * @title Table merge cells
 * @brief Span a cell across two columns by setting the `merge_right` control flag.
 *
 * The `ctrl` attribute on `<lv_table-cell>` takes one or more flags joined
 * with `|`. `merge_right` extends the cell into the column immediately to
 * its right, hiding that neighbour's content — the header row here merges
 * column 0 into column 1 so the title spans both quarters. To span more
 * than two columns, chain `merge_right` on each adjacent cell except the
 * last. `text_crop` is a useful companion flag when a long value would
 * overflow the merged cell.
 */
void lv_example_table_merge_cells_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Add a third column plus another `merge_right` on the header to span it; without the chain the title stops at column 1. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Table: merged cells");

    lv_obj_t * lv_table_0 = lv_table_create(screen);
    lv_table_set_cell_value(lv_table_0, 0, 0, "Quarterly summary");
    lv_table_set_cell_ctrl(lv_table_0, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(lv_table_0, 1, 0, "Q1");
    lv_table_set_cell_value(lv_table_0, 1, 1, "Q2");
    lv_table_set_cell_value(lv_table_0, 2, 0, "120");
    lv_table_set_cell_value(lv_table_0, 2, 1, "145");
}

