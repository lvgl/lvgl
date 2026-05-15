/**
 * @file lv_example_table_cells.c
 */

#include "../../../../lvgl.h"

/**
 * @title Table cell values
 * @brief Populate a grid of cells with text using `<lv_table-cell>` children.
 *
 * Each cell is declared with its `row`/`column` (zero-based) and a `value`
 * string. Cells default to empty, so only the populated coordinates have
 * visible text. `row_count`/`column_count` size the grid up front so the
 * cells render in the right slots even if their declaration order doesn't
 * walk row-major.
 */
void lv_example_table_cells_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Add another `<lv_table-cell>` with different `row`/`column` to populate more cells. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Table: cell values");

    lv_obj_t * lv_table_0 = lv_table_create(screen);
    lv_table_set_row_count(lv_table_0, 3);
    lv_table_set_column_count(lv_table_0, 2);
    lv_table_set_cell_value(lv_table_0, 0, 0, "City");
    lv_table_set_cell_value(lv_table_0, 0, 1, "Population");
    lv_table_set_cell_value(lv_table_0, 1, 0, "Berlin");
    lv_table_set_cell_value(lv_table_0, 1, 1, "3.7M");
    lv_table_set_cell_value(lv_table_0, 2, 0, "Paris");
    lv_table_set_cell_value(lv_table_0, 2, 1, "2.1M");
}

