/**
 * @file lv_example_scroll_floating.c
 */

#include "../../../lvgl.h"

/**
 * @title Floating button over a list
 * @brief A floating child stays pinned while the list behind it scrolls.
 *
 * The panel is a scrollable list of buttons. The round "+" button sets
 * `floating="true"`: a floating child is ignored by the layout and is not
 * moved when its parent scrolls, so it stays anchored to the bottom-right
 * corner while the list slides underneath it.
 */
void lv_example_scroll_floating_create(void)
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Scroll the list — the round + button has `floating="true"` so it stays pinned. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Scroll: floating button");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 240, 180);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_1, 8, 0);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_0, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Track 1");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_1, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Track 2");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_2, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "Track 3");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_3, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "Track 4");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_4, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "Track 5");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_5, lv_pct(100));
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_6, "Track 6");

    /* Pinned to bottom-right, unaffected by the list's scroll */
    lv_obj_t * lv_button_6 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_6, 44, 44);
    lv_obj_set_align(lv_button_6, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_flag(lv_button_6, LV_OBJ_FLAG_FLOATING, true);
    lv_obj_set_style_bg_color(lv_button_6, lv_color_hex(0x9429ff), 0);
    lv_obj_add_style(lv_button_6, &style_fab, 0);
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_6);
    lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_7, "+");
}

