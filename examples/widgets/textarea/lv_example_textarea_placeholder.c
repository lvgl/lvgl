/**
 * @file lv_example_textarea_placeholder.c
 */

#include "../../lv_examples.h"
#if LV_USE_TEXTAREA && LV_BUILD_EXAMPLES

/**
 * @title Text area placeholder
 * @brief Show a hint while the text area is empty.
 *
 * Both text areas carry the same `placeholder_text`. The first is left empty
 * so the grey hint is visible; the second has `text` set, which hides the
 * placeholder — the contrast shows exactly when the hint appears.
 */
void lv_example_textarea_placeholder(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Empty: placeholder hint is shown */
    lv_obj_t * textarea_1 = lv_textarea_create(screen);
    lv_obj_set_width(textarea_1, lv_pct(60));
    lv_textarea_set_one_line(textarea_1, true);
    lv_textarea_set_placeholder_text(textarea_1, "Search...");

    /* Filled: placeholder is hidden */
    lv_obj_t * textarea_2 = lv_textarea_create(screen);
    lv_obj_set_width(textarea_2, lv_pct(60));
    lv_textarea_set_one_line(textarea_2, true);
    lv_textarea_set_placeholder_text(textarea_2, "Search…");
    lv_textarea_set_text(textarea_2, "Hello world!");
}
#endif
