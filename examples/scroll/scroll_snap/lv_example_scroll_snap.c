/**
 * @file lv_example_scroll_snap.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Scroll snapping
 * @brief Snap children to the centre, and opt one child out of snapping.
 *
 * The row panel sets `scroll_snap_x="center"`, so when a drag is released
 * the nearest panel animates to the centre. The third button sets
 * `snappable="false"`, removing it from the snap candidates: scrolling
 * glides over it and lands on a neighbouring, snappable panel instead.
 */
void lv_example_scroll_snap(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Release a drag — panels snap to centre; "No snap" is skipped because `snappable="false"`. */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 290, 150);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(lv_obj_1, 10, 0);
    lv_obj_set_scroll_snap_x(lv_obj_1, LV_SCROLL_SNAP_CENTER);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_0, 120, lv_pct(100));
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "Panel 1");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_1, 120, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Panel 2");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_2, 120, lv_pct(100));
    lv_obj_set_flag(lv_button_2, LV_OBJ_FLAG_SNAPPABLE, false);
    lv_obj_set_style_bg_color(lv_button_2, lv_color_hex(0x9429ff), 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "No snap");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_3, 120, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "Panel 4");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_4, 120, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "Panel 5");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_5, 120, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "Panel 6");
}
#endif
