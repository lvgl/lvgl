/**
 * @file lv_example_table_rows_columns.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

/**
 * @title Table rows and columns
 * @brief Set the table dimensions explicitly so the grid is sized before cells are placed.
 *
 * `row_count` and `column_count` give the table its shape. Cells without
 * declared values still occupy a slot in the grid, so the visible empty
 * rows in the example are part of the layout, not leftover space. Here a
 * 10×2 grid is declared and only a handful of cells are populated — the
 * gaps between them stay rendered as empty cells. Resizing the row/column
 * counts grows or shrinks the grid; cells outside the new bounds simply
 * don't render. When the row count is tall enough that the grid no longer
 * fits the widget's height, the table becomes scrollable.
 */
void lv_example_table_rows_columns(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Bump `row_count` past what fits on screen — the table scrolls; the empty cells stay in place because they're part of the grid. */
    lv_obj_t * table = lv_table_create(screen);
    lv_obj_set_align(table, LV_ALIGN_CENTER);
    lv_table_set_row_count(table, 10);
    lv_table_set_column_count(table, 2);
    lv_obj_set_height(table, lv_pct(90));
    lv_table_set_cell_value(table, 0, 0, "A");
    lv_table_set_cell_value(table, 0, 1, "B");
    lv_table_set_cell_value(table, 2, 1, "middle");
    lv_table_set_cell_value(table, 8, 0, "8th");
    lv_table_set_cell_value(table, 9, 0, "last row");
}
#endif
