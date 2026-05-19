/**
 * @file lv_example_scroll_scrollable.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Disabling scrolling
 * @brief Clearing the SCROLLABLE flag clips overflowing content instead of scrolling it.
 *
 * Both panels hold the same overflowing column of buttons. The left one
 * keeps the default behaviour and scrolls. The right one sets
 * `scrollable="false"`, so the flag is cleared: the extra content is
 * simply clipped and the panel cannot be scrolled at all.
 */
void lv_example_scroll_scrollable(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Left panel scrolls; the right has `scrollable="false"` so the same content is just clipped. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Scroll: scrollable flag");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, lv_pct(100), 160);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(lv_obj_1, 16, 0);
    /* Default: scrollable */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_2, 130, lv_pct(100));
    lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_2, 8, 0);
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_1, "scrollable");

    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_0, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "A");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_1, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "B");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_2, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "C");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_3, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "D");

    /* Flag cleared: content is clipped, no scrolling */
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_3, 130, lv_pct(100));
    lv_obj_set_flex_flow(lv_obj_3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_3, 8, 0);
    lv_obj_set_flag(lv_obj_3, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_border_color(lv_obj_3, lv_color_hex(0x9429ff), 0);
    lv_obj_set_style_border_width(lv_obj_3, 2, 0);
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_6, "clipped");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_4, lv_pct(100));
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_7, "A");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_5, lv_pct(100));
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_8, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_8, "B");

    lv_obj_t * lv_button_6 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_6, lv_pct(100));
    lv_obj_t * lv_label_9 = lv_label_create(lv_button_6);
    lv_obj_set_align(lv_label_9, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_9, "C");

    lv_obj_t * lv_button_7 = lv_button_create(lv_obj_3);
    lv_obj_set_width(lv_button_7, lv_pct(100));
    lv_obj_t * lv_label_10 = lv_label_create(lv_button_7);
    lv_obj_set_align(lv_label_10, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_10, "D");
}
#endif
