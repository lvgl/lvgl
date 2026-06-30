/**
 * @file lv_example_scroll_scrollable.c
 */

#include "../lv_examples.h"
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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Left panel scrolls; the right has `scrollable="false"` so the same content is just clipped. */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, lv_pct(100), 160);
    lv_obj_set_flex_flow(container_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(container_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(container_1, 16, 0);
    /* Default: scrollable */
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_size(container_2, 130, lv_pct(100));
    lv_obj_set_flex_flow(container_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container_2, 8, 0);
    lv_obj_t * label_1 = lv_label_create(container_2);
    lv_label_set_text(label_1, "scrollable");

    lv_obj_t * button_1 = lv_button_create(container_2);
    lv_obj_set_width(button_1, lv_pct(100));
    lv_obj_t * label_2 = lv_label_create(button_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "A");

    lv_obj_t * button_2 = lv_button_create(container_2);
    lv_obj_set_width(button_2, lv_pct(100));
    lv_obj_t * label_3 = lv_label_create(button_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "B");

    lv_obj_t * button_3 = lv_button_create(container_2);
    lv_obj_set_width(button_3, lv_pct(100));
    lv_obj_t * label_4 = lv_label_create(button_3);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "C");

    lv_obj_t * button_4 = lv_button_create(container_2);
    lv_obj_set_width(button_4, lv_pct(100));
    lv_obj_t * label_5 = lv_label_create(button_4);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "D");

    /* Flag cleared: content is clipped, no scrolling */
    lv_obj_t * container_3 = lv_obj_create(container_1);
    lv_obj_set_size(container_3, 130, lv_pct(100));
    lv_obj_set_flex_flow(container_3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container_3, 8, 0);
    lv_obj_set_flag(container_3, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_border_color(container_3, lv_color_hex(0x9429ff), 0);
    lv_obj_set_style_border_width(container_3, 2, 0);
    lv_obj_t * label_6 = lv_label_create(container_3);
    lv_label_set_text(label_6, "clipped");

    lv_obj_t * button_5 = lv_button_create(container_3);
    lv_obj_set_width(button_5, lv_pct(100));
    lv_obj_t * label_7 = lv_label_create(button_5);
    lv_obj_set_align(label_7, LV_ALIGN_CENTER);
    lv_label_set_text(label_7, "A");

    lv_obj_t * button_6 = lv_button_create(container_3);
    lv_obj_set_width(button_6, lv_pct(100));
    lv_obj_t * label_8 = lv_label_create(button_6);
    lv_obj_set_align(label_8, LV_ALIGN_CENTER);
    lv_label_set_text(label_8, "B");

    lv_obj_t * button_7 = lv_button_create(container_3);
    lv_obj_set_width(button_7, lv_pct(100));
    lv_obj_t * label_9 = lv_label_create(button_7);
    lv_obj_set_align(label_9, LV_ALIGN_CENTER);
    lv_label_set_text(label_9, "C");

    lv_obj_t * button_8 = lv_button_create(container_3);
    lv_obj_set_width(button_8, lv_pct(100));
    lv_obj_t * label_10 = lv_label_create(button_8);
    lv_obj_set_align(label_10, LV_ALIGN_CENTER);
    lv_label_set_text(label_10, "D");
}
#endif
