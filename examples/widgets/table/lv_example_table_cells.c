/**
 * @file lv_example_table_cells.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

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
void lv_example_table_cells(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Add another `<lv_table-cell>` with different `row`/`column` to populate more cells. */
    lv_obj_t * table = lv_table_create(screen);
    lv_table_set_row_count(table, 3);
    lv_table_set_column_count(table, 2);
    lv_obj_set_align(table, LV_ALIGN_CENTER);
    lv_table_set_cell_value(table, 0, 0, "City");
    lv_table_set_cell_value(table, 0, 1, "Population");
    lv_table_set_cell_value(table, 1, 0, "Berlin");
    lv_table_set_cell_value(table, 1, 1, "3.7M");
    lv_table_set_cell_value(table, 2, 0, "Paris");
    lv_table_set_cell_value(table, 2, 1, "2.1M");
}
#endif
