/**
 * @file lv_example_table_width.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit the `width` values to redistribute horizontal space between columns. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Table: column widths");

    lv_obj_t * lv_table_0 = lv_table_create(screen);
    lv_table_set_row_count(lv_table_0, 3);
    lv_table_set_column_count(lv_table_0, 3);
    lv_table_set_column_width(lv_table_0, 0, 50);
    lv_table_set_column_width(lv_table_0, 1, 180);
    lv_table_set_column_width(lv_table_0, 2, 80);
    lv_table_set_cell_value(lv_table_0, 0, 0, "No.");
    lv_table_set_cell_value(lv_table_0, 0, 1, "Description");
    lv_table_set_cell_value(lv_table_0, 0, 2, "Price");
    lv_table_set_cell_value(lv_table_0, 1, 0, "1");
    lv_table_set_cell_value(lv_table_0, 1, 1, "Espresso, small");
    lv_table_set_cell_value(lv_table_0, 1, 2, "2.50");
    lv_table_set_cell_value(lv_table_0, 2, 0, "2");
    lv_table_set_cell_value(lv_table_0, 2, 1, "Cappuccino, regular");
    lv_table_set_cell_value(lv_table_0, 2, 2, "3.80");
}
#endif
