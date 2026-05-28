/**
 * @file lv_example_scroll_chain.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Scroll chaining
 * @brief A nested scrollable can stop scrolling from propagating to its parent.
 *
 * The outer panel scrolls, and it contains an inner scrollable box that
 * also overflows. By default, reaching the inner box's edge would "chain"
 * the remaining scroll to the outer panel. Setting `scroll_chain="false"`
 * on the inner box breaks that link, so scrolling stops at the box and
 * the outer panel does not move.
 */
void lv_example_scroll_chain(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* Outer scrollable panel */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, 240, 180);
    lv_obj_set_flex_flow(container_1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container_1, 8, 0);
    lv_obj_t * button_1 = lv_button_create(container_1);
    lv_obj_set_width(button_1, lv_pct(100));
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Outer 1");

    /* Inner scrollable box; chaining to the parent disabled */
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_size(container_2, lv_pct(100), 90);
    lv_obj_set_flex_flow(container_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container_2, 6, 0);
    lv_obj_set_flag(container_2, LV_OBJ_FLAG_SCROLL_CHAIN, false);
    lv_obj_set_style_border_color(container_2, lv_color_hex(0x9429ff), 0);
    lv_obj_set_style_border_width(container_2, 2, 0);
    lv_obj_t * button_2 = lv_button_create(container_2);
    lv_obj_set_width(button_2, lv_pct(100));
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Inner 1");

    lv_obj_t * button_3 = lv_button_create(container_2);
    lv_obj_set_width(button_3, lv_pct(100));
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Inner 2");

    lv_obj_t * button_4 = lv_button_create(container_2);
    lv_obj_set_width(button_4, lv_pct(100));
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Inner 3");

    lv_obj_t * button_5 = lv_button_create(container_2);
    lv_obj_set_width(button_5, lv_pct(100));
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Inner 4");

    lv_obj_t * button_6 = lv_button_create(container_1);
    lv_obj_set_width(button_6, lv_pct(100));
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Outer 2");

    lv_obj_t * button_7 = lv_button_create(container_1);
    lv_obj_set_width(button_7, lv_pct(100));
    lv_obj_t * label_7 = lv_label_create(button_7);
    lv_obj_set_align(label_7, LV_ALIGN_CENTER);
    lv_label_set_text(label_7, "Outer 3");

    lv_obj_t * button_8 = lv_button_create(container_1);
    lv_obj_set_width(button_8, lv_pct(100));
    lv_obj_t * label_8 = lv_label_create(button_8);
    lv_obj_set_align(label_8, LV_ALIGN_CENTER);
    lv_label_set_text(label_8, "Outer 4");
}
#endif
