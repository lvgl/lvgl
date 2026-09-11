/**
 * @file lv_example_table_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

/**
 * @title Table styling
 * @brief Style the table background and the per-cell appearance.
 *
 * Table has two relevant parts: `MAIN` (the background and overall frame)
 * and `ITEMS` (each cell — borders, padding, text). Attaching a named
 * `style_items` style covers every cell uniformly. To make the header row
 * stand out, attach a second style with `selector="items"` and the
 * row-specific state — but XML doesn't yet have a per-row selector; instead
 * set the header row's cell `ctrl` flags or restyle in code.
 */
void lv_example_table_styling(void)
{
    static lv_style_t style_table_main;
    static lv_style_t style_table_items;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_table_main);
        lv_style_set_bg_color(&style_table_main, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_table_main, (255 * 100 / 100));
        lv_style_set_radius(&style_table_main, 8);
        lv_style_set_border_color(&style_table_main, lv_color_hex(0xe5e7eb));
        lv_style_set_border_width(&style_table_main, 1);

        lv_style_init(&style_table_items);
        lv_style_set_bg_color(&style_table_items, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_table_items, (255 * 100 / 100));
        lv_style_set_border_color(&style_table_items, lv_color_hex(0xe5e7eb));
        lv_style_set_border_width(&style_table_items, 1);
        lv_style_set_pad_all(&style_table_items, 8);
        lv_style_set_text_color(&style_table_items, lv_color_hex(0x1345b0));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Recolour `style_table_items` to apply a different look to every cell at once. */
    lv_obj_t * table = lv_table_create(screen);
    lv_obj_set_align(table, LV_ALIGN_CENTER);
    lv_table_set_row_count(table, 3);
    lv_table_set_column_count(table, 2);
    lv_table_set_cell_value(table, 0, 0, "Metric");
    lv_table_set_cell_value(table, 0, 1, "Value");
    lv_table_set_cell_value(table, 1, 0, "Latency");
    lv_table_set_cell_value(table, 1, 1, "14 ms");
    lv_table_set_cell_value(table, 2, 0, "Throughput");
    lv_table_set_cell_value(table, 2, 1, "98 req/s");
    lv_obj_add_style(table, &style_table_main, LV_PART_MAIN);
    lv_obj_add_style(table, &style_table_items, LV_PART_ITEMS);
}
#endif
