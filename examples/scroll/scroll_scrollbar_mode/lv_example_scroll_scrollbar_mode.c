/**
 * @file lv_example_scroll_scrollbar_mode.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x9429ff)

/**
 * @title Scrollbar mode
 * @brief Compare an always-hidden scrollbar with an always-visible one.
 *
 * Two identical overflowing panels differ only in `scrollbar_mode`: the
 * left one is `off` (scrollable, but the bar is never drawn), the right
 * one is `on` (the bar is always drawn even when idle). The other modes
 * are `auto` (bar only while scrollable) and `active` (bar only while
 * actively scrolling).
 */
void lv_example_scroll_scrollbar_mode(void)
{
    static lv_style_t style_scrollbar;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_scrollbar);
        lv_style_set_width(&style_scrollbar, 10);
        lv_style_set_radius(&style_scrollbar, 5);
        lv_style_set_bg_color(&style_scrollbar, ACCENT);
        lv_style_set_bg_opa(&style_scrollbar, (255 * 100 / 100));
        lv_style_set_pad_right(&style_scrollbar, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Both panels scroll; only the right one ever shows a scrollbar. */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, lv_pct(100), 160);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(lv_obj_1, 16, 0);
    /* scrollbar_mode="off": scrolls, bar never shown */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_2, 130, lv_pct(100));
    lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_2, 8, 0);
    lv_obj_set_scrollbar_mode(lv_obj_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_0, "mode: off");

    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_0, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "A");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_1, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "B");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_2, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "C");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_3, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "D");

    /* scrollbar_mode="on": bar always visible */
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_3, 130, lv_pct(100));
    lv_obj_set_flex_flow(lv_obj_3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_3, 8, 0);
    lv_obj_set_scrollbar_mode(lv_obj_3, LV_SCROLLBAR_MODE_ON);
    lv_obj_add_style(lv_obj_3, &style_scrollbar, LV_PART_SCROLLBAR);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_5, "mode: on");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_4, lv_pct(100));
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_6, "A");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_5, lv_pct(100));
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_7, "B");

    lv_obj_t * lv_button_6 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_6, lv_pct(100));
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_6);
    lv_obj_set_align(lv_label_8, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_8, "C");

    lv_obj_t * lv_button_7 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_7, lv_pct(100));
    lv_obj_t * lv_label_9 = lv_label_create(lv_button_7);
    lv_obj_set_align(lv_label_9, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_9, "D");
}
#endif
