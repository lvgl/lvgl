/**
 * @file lv_example_spinner_styling.c
 */

#include "../../../../lvgl.h"

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
void lv_example_spinner_styling_create(void)
{
    static lv_style_t style_spinner_main;
    static lv_style_t style_spinner_indicator;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_spinner_main);
        lv_style_set_arc_color(&style_spinner_main, lv_color_hex(0xe5e7eb));
        lv_style_set_arc_width(&style_spinner_main, 8);
        lv_style_set_arc_rounded(&style_spinner_main, true);

        lv_style_init(&style_spinner_indicator);
        lv_style_set_arc_color(&style_spinner_indicator, lv_color_hex(0x6366f1));
        lv_style_set_arc_width(&style_spinner_indicator, 8);
        lv_style_set_arc_rounded(&style_spinner_indicator, true);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Recolour `style_table_items` to apply a different look to every cell at once. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Spinner: styling parts");

    lv_obj_t * lv_spinner_0 = lv_spinner_create(screen);
    lv_obj_set_size(lv_spinner_0, 100, 100);
    lv_spinner_set_anim_duration(lv_spinner_0, 1200);
    lv_spinner_set_arc_sweep(lv_spinner_0, 270);
    lv_obj_add_style(lv_spinner_0, &style_spinner_main, LV_PART_MAIN);
    lv_obj_add_style(lv_spinner_0, &style_spinner_indicator, LV_PART_INDICATOR);
}

