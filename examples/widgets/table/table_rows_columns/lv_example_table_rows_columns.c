/**
 * @file lv_example_table_rows_columns.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Bump `row_count` past what fits on screen — the table scrolls; the empty cells stay in place because they're part of the grid. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Table: rows and columns");

    lv_obj_t * lv_table_0 = lv_table_create(screen);
    lv_table_set_row_count(lv_table_0, 10);
    lv_table_set_column_count(lv_table_0, 2);
    lv_table_set_cell_value(lv_table_0, 0, 0, "A");
    lv_table_set_cell_value(lv_table_0, 0, 1, "B");
    lv_table_set_cell_value(lv_table_0, 2, 1, "middle");
    lv_table_set_cell_value(lv_table_0, 8, 0, "8th");
    lv_table_set_cell_value(lv_table_0, 9, 0, "last row");
}
#endif
