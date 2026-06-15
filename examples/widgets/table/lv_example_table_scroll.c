/**
 * @file lv_example_table_scroll.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

/**
 * @title Table scrolling
 * @brief A table whose content grid is taller than its explicit height becomes scrollable.
 *
 * The table itself handles scrolling — when its `height` is smaller than
 * the height needed to draw all rows, the rest are reachable by dragging.
 * With `height="LV_SIZE_CONTENT"` (the default), the table grows to fit
 * every row and the parent decides if it scrolls. Here we fix the height
 * so the bottom rows are off-screen and the user has to scroll the table
 * itself.
 */
void lv_example_table_scroll(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Drag the table vertically to reveal the lower rows. */
    lv_obj_t * table = lv_table_create(screen);
    lv_obj_set_align(table, LV_ALIGN_CENTER);
    lv_obj_set_size(table, 240, 160);
    lv_table_set_row_count(table, 12);
    lv_table_set_column_count(table, 2);
    lv_table_set_column_width(table, 0, 70);
    lv_table_set_column_width(table, 1, 160);
    lv_table_set_cell_value(table, 0, 0, "No.");
    lv_table_set_cell_value(table, 0, 1, "Item");
    lv_table_set_cell_value(table, 1, 0, "1");
    lv_table_set_cell_value(table, 1, 1, "Wrench");
    lv_table_set_cell_value(table, 2, 0, "2");
    lv_table_set_cell_value(table, 2, 1, "Screwdriver");
    lv_table_set_cell_value(table, 3, 0, "3");
    lv_table_set_cell_value(table, 3, 1, "Hammer");
    lv_table_set_cell_value(table, 4, 0, "4");
    lv_table_set_cell_value(table, 4, 1, "Drill");
    lv_table_set_cell_value(table, 5, 0, "5");
    lv_table_set_cell_value(table, 5, 1, "Saw");
    lv_table_set_cell_value(table, 6, 0, "6");
    lv_table_set_cell_value(table, 6, 1, "Pliers");
    lv_table_set_cell_value(table, 7, 0, "7");
    lv_table_set_cell_value(table, 7, 1, "Tape measure");
    lv_table_set_cell_value(table, 8, 0, "8");
    lv_table_set_cell_value(table, 8, 1, "Level");
    lv_table_set_cell_value(table, 9, 0, "9");
    lv_table_set_cell_value(table, 9, 1, "Crowbar");
    lv_table_set_cell_value(table, 10, 0, "10");
    lv_table_set_cell_value(table, 10, 1, "Mallet");
    lv_table_set_cell_value(table, 11, 0, "11");
    lv_table_set_cell_value(table, 11, 1, "Chisel");
}
#endif
