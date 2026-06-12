/**
 * @file lv_example_table_width.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

/**
 * @title Table column widths
 * @brief Set per-column pixel widths through `<lv_table-column>` children.
 *
 * By default every column gets the same width (the table's content area
 * divided by `column_count`). A `<lv_table-column>` child overrides that for
 * a single column: pick a column index and give it a `width` in pixels.
 * Mixing default-width and explicit-width columns is fine — only the ones
 * you address change.
 */
void lv_example_table_width(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit the `width` values to redistribute horizontal space between columns. */
    lv_obj_t * table = lv_table_create(screen);
    lv_table_set_row_count(table, 3);
    lv_table_set_column_count(table, 3);
    lv_table_set_column_width(table, 0, 60);
    lv_table_set_column_width(table, 1, 180);
    lv_table_set_column_width(table, 2, 80);
    lv_table_set_cell_value(table, 0, 0, "No.");
    lv_table_set_cell_value(table, 0, 1, "Description");
    lv_table_set_cell_value(table, 0, 2, "Price");
    lv_table_set_cell_value(table, 1, 0, "1");
    lv_table_set_cell_value(table, 1, 1, "Espresso, small");
    lv_table_set_cell_value(table, 1, 2, "2.50");
    lv_table_set_cell_value(table, 2, 0, "2");
    lv_table_set_cell_value(table, 2, 1, "Cappuccino, regular");
    lv_table_set_cell_value(table, 2, 2, "3.80");
}
#endif
