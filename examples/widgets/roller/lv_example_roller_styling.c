/**
 * @file lv_example_roller_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_ROLLER && LV_BUILD_EXAMPLES

/**
 * @title Roller styling
 * @brief Style the main listbox and the selected item; use `text_line_space` to set the selected band's height.
 *
 * Roller has two relevant parts: `MAIN` (the whole listbox, where unselected
 * items are drawn) and `SELECTED` (the highlighted row in the middle). Name a
 * style for each and attach them with `selector="main"` and
 * `selector="selected"`.
 *
 * `text_line_space` on MAIN does more than space the text rows: because the
 * selected indicator always sits in the gap between two list items, growing
 * `text_line_space` directly grows the height of the highlighted band — so
 * the selected row gets visually taller without a separate height setting.
 */
void lv_example_roller_styling(void)
{
    LV_FONT_DECLARE(font_example_large);

    static lv_style_t style_roller_main;
    static lv_style_t style_roller_selected;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_roller_main);
        lv_style_set_bg_color(&style_roller_main, lv_color_hex(0xf3f4f6));
        lv_style_set_bg_opa(&style_roller_main, (255 * 100 / 100));
        lv_style_set_radius(&style_roller_main, 12);
        lv_style_set_border_color(&style_roller_main, lv_color_hex(0xd1d5db));
        lv_style_set_border_width(&style_roller_main, 1);
        lv_style_set_text_color(&style_roller_main, lv_color_hex(0x6b7280));
        lv_style_set_text_line_space(&style_roller_main, 20);

        lv_style_init(&style_roller_selected);
        lv_style_set_bg_color(&style_roller_selected, lv_color_hex(0x6366f1));
        lv_style_set_bg_opa(&style_roller_selected, (255 * 100 / 100));
        lv_style_set_text_color(&style_roller_selected, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_roller_selected, &font_example_large);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Bump `text_line_space` on `style_roller_main` to grow the selected band's height — the indicator always fills the gap between rows. */
    lv_obj_t * roller = lv_roller_create(screen);
    lv_obj_set_width(roller, 160);
    lv_roller_set_visible_row_count(roller, 5);
    lv_roller_set_options(roller, "Berlin\nParis\nTokyo\nSydney\nNew York", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller, 2, false);
    lv_obj_add_style(roller, &style_roller_main, LV_PART_MAIN);
    lv_obj_add_style(roller, &style_roller_selected, LV_PART_SELECTED);
}
#endif
