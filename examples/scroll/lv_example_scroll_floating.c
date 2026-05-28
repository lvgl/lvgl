/**
 * @file lv_example_scroll_floating.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Floating button over a list
 * @brief A floating child stays pinned while the list behind it scrolls.
 *
 * The panel is a scrollable list of buttons. The round "+" button sets
 * `floating="true"`: a floating child is ignored by the layout and is not
 * moved when its parent scrolls, so it stays anchored to the bottom-right
 * corner while the list slides underneath it.
 */
void lv_example_scroll_floating(void)
{
    static lv_style_t style_fab;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_fab);
        lv_style_set_radius(&style_fab, 22);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Scroll the list — the round + button has `floating="true"` so it stays pinned. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 240, 180);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container, 8, 0);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_width(button_1, lv_pct(100));
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Track 1");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_width(button_2, lv_pct(100));
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Track 2");

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_width(button_3, lv_pct(100));
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Track 3");

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_width(button_4, lv_pct(100));
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Track 4");

    lv_obj_t * button_5 = lv_button_create(container);
    lv_obj_set_width(button_5, lv_pct(100));
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Track 5");

    lv_obj_t * button_6 = lv_button_create(container);
    lv_obj_set_width(button_6, lv_pct(100));
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Track 6");

    /* Pinned to bottom-right, unaffected by the list's scroll */
    lv_obj_t * button_7 = lv_button_create(container);
    lv_obj_set_size(button_7, 44, 44);
    lv_obj_set_align(button_7, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_flag(button_7, LV_OBJ_FLAG_FLOATING, true);
    lv_obj_set_style_bg_color(button_7, lv_color_hex(0x9429ff), 0);
    lv_obj_add_style(button_7, &style_fab, 0);
    lv_obj_t * label_7 = lv_label_create(button_7);
    lv_obj_set_align(label_7, LV_ALIGN_CENTER);
    lv_label_set_text(label_7, "+");
}
#endif
