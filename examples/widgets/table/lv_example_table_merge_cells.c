/**
 * @file lv_example_table_merge_cells.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

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
void lv_example_table_merge_cells(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Add a third column plus another `merge_right` on the header to span it; without the chain the title stops at column 1. */
    lv_obj_t * table = lv_table_create(screen);
    lv_obj_set_align(table, LV_ALIGN_CENTER);
    lv_table_set_cell_value(table, 0, 0, "Quarterly summary");
    lv_table_set_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, 1, 0, "Q1");
    lv_table_set_cell_value(table, 1, 1, "Q2");
    lv_table_set_cell_value(table, 2, 0, "120");
    lv_table_set_cell_value(table, 2, 1, "145");
}
#endif
