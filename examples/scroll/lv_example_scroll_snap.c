/**
 * @file lv_example_scroll_snap.c
 */

#include "../lv_examples.h"
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
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 290, 150);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container, 10, 0);
    lv_obj_set_scroll_snap_x(container, LV_SCROLL_SNAP_CENTER);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_size(button_1, 120, lv_pct(100));
    lv_obj_set_flag(button_1, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Panel 1");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_size(button_2, 120, lv_pct(100));
    lv_obj_set_flag(button_2, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Panel 2");

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_size(button_3, 120, lv_pct(100));
    lv_obj_set_flag(button_3, LV_OBJ_FLAG_SNAPPABLE, false);
    lv_obj_set_style_bg_color(button_3, lv_color_hex(0x9429ff), 0);
    lv_obj_set_flag(button_3, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "No snap");

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_size(button_4, 120, lv_pct(100));
    lv_obj_set_flag(button_4, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Panel 4");

    lv_obj_t * button_5 = lv_button_create(container);
    lv_obj_set_size(button_5, 120, lv_pct(100));
    lv_obj_set_flag(button_5, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Panel 5");

    lv_obj_t * button_6 = lv_button_create(container);
    lv_obj_set_size(button_6, 120, lv_pct(100));
    lv_obj_set_flag(button_6, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Panel 6");
}
#endif
