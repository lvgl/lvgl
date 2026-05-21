/**
 * @file lv_example_scroll_chain.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Scroll inside the inner box: at its edge the outer panel stays put because `scroll_chain="false"`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Scroll: chaining");

    /* Outer scrollable panel */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 240, 180);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_1, 8, 0);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_0, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Outer 1");

    /* Inner scrollable box; chaining to the parent disabled */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_2, lv_pct(100), 90);
    lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_obj_2, 6, 0);
    lv_obj_set_flag(lv_obj_2, LV_OBJ_FLAG_SCROLL_CHAIN, false);
    lv_obj_set_style_border_color(lv_obj_2, lv_color_hex(0x9429ff), 0);
    lv_obj_set_style_border_width(lv_obj_2, 2, 0);
    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_1, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Inner 1");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_2, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "Inner 2");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_3, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "Inner 3");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_2);
    lv_obj_set_width(lv_button_4, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "Inner 4");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_5, lv_pct(100));
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_6, "Outer 2");

    lv_obj_t * lv_button_6 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_6, lv_pct(100));
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_6);
    lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_7, "Outer 3");

    lv_obj_t * lv_button_7 = lv_button_create(lv_obj_1);
    lv_obj_set_width(lv_button_7, lv_pct(100));
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_7);
    lv_obj_set_align(lv_label_8, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_8, "Outer 4");
}
#endif
