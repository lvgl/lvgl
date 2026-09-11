/**
 * @file lv_example_spinner_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPINNER && LV_BUILD_EXAMPLES

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
void lv_example_spinner_styling(void)
{
    static lv_style_t style_table_main;
    static lv_style_t style_table_items;
    static lv_style_t style_table_items_pressed;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_table_main);
        lv_style_set_bg_color(&style_table_main, lv_color_hex(0xffffff));
        lv_style_set_border_color(&style_table_main, lv_color_hex(0xe5e7eb));
        lv_style_set_border_width(&style_table_main, 2);
        lv_style_set_clip_corner(&style_table_main, true);

        lv_style_init(&style_table_items);
        lv_style_set_bg_color(&style_table_items, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_table_items, (255 * 100 / 100));
        lv_style_set_border_color(&style_table_items, lv_color_hex(0xe5e7eb));
        lv_style_set_border_width(&style_table_items, 1);
        lv_style_set_border_side(&style_table_items, LV_BORDER_SIDE_FULL);
        lv_style_set_pad_all(&style_table_items, 6);
        lv_style_set_text_align(&style_table_items, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_color(&style_table_items, lv_color_hex(0x111827));

        lv_style_init(&style_table_items_pressed);
        lv_style_set_bg_color(&style_table_items_pressed, lv_color_hex(0x6366f1));
        lv_style_set_text_color(&style_table_items_pressed, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Recolour `style_table_items` to apply a different look to every cell at once. */
    lv_obj_t * table = lv_table_create(screen);
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
    lv_obj_add_style(table, &style_table_items_pressed, LV_PART_ITEMS | LV_STATE_PRESSED);
}
#endif
