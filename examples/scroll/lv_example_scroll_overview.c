/**
 * @file lv_example_scroll_overview.c
 */

#include "../lv_examples.h"
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
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 220, 140);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container, 8, 0);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_width(button_1, lv_pct(100));
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Item 1");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_width(button_2, lv_pct(100));
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Item 2");

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_width(button_3, lv_pct(100));
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Item 3");

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_width(button_4, lv_pct(100));
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Item 4");

    lv_obj_t * button_5 = lv_button_create(container);
    lv_obj_set_width(button_5, lv_pct(100));
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Item 5");

    lv_obj_t * button_6 = lv_button_create(container);
    lv_obj_set_width(button_6, lv_pct(100));
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Item 6");
}
#endif
