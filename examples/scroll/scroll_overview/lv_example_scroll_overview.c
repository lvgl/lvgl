/**
 * @file lv_example_scroll_overview.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Scroll basics
 * @brief A panel becomes scrollable on its own when its children overflow it.
 *
 * The panel is only 140 px tall but stacks six full-width buttons, so the
 * content is taller than the panel. LVGL makes the panel scrollable and
 * shows a vertical scrollbar automatically — no scroll-specific API is
 * used here, only content that exceeds the bounds.
 */
void lv_example_scroll_overview(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* Fixed-height panel; the column of buttons overflows it */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 220, 140);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_1, 8, 0);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_0, lv_pct(100));
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "Item 1");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_1, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Item 2");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_2, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Item 3");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_3, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "Item 4");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_4, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "Item 5");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_5, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "Item 6");
}
#endif
