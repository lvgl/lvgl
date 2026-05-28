/**
 * @file lv_example_scroll_one.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Scroll one at a time
 * @brief Restrict each scroll gesture to a single snappable child.
 *
 * The panel combines centre snapping with `scroll_one="true"`. Snapping
 * alone lets a fast fling cross several panels before settling; with
 * "scroll one" enabled every gesture advances by exactly one panel, no
 * matter how hard it is thrown. It requires snappable children and a
 * snap mode other than `none`.
 */
void lv_example_scroll_one(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Fling hard — it still advances exactly one panel because `scroll_one="true"`. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 300, 150);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container, 10, 0);
    lv_obj_set_scroll_snap_x(container, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flag(container, LV_OBJ_FLAG_SCROLL_ONE, true);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_size(button_1, 80, lv_pct(100));
    lv_obj_set_flag(button_1, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Panel 1");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_size(button_2, 80, lv_pct(100));
    lv_obj_set_flag(button_2, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Panel 2");

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_size(button_3, 80, lv_pct(100));
    lv_obj_set_flag(button_3, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Panel 3");

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_size(button_4, 80, lv_pct(100));
    lv_obj_set_flag(button_4, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Panel 4");

    lv_obj_t * button_5 = lv_button_create(container);
    lv_obj_set_size(button_5, 80, lv_pct(100));
    lv_obj_set_flag(button_5, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Panel 5");

    lv_obj_t * button_6 = lv_button_create(container);
    lv_obj_set_size(button_6, 80, lv_pct(100));
    lv_obj_set_flag(button_6, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Panel 6");

    lv_obj_t * button_7 = lv_button_create(container);
    lv_obj_set_size(button_7, 80, lv_pct(100));
    lv_obj_set_flag(button_7, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_7 = lv_label_create(button_7);
    lv_obj_set_align(label_7, LV_ALIGN_CENTER);
    lv_label_set_text(label_7, "Panel 7");

    lv_obj_t * button_8 = lv_button_create(container);
    lv_obj_set_size(button_8, 80, lv_pct(100));
    lv_obj_set_flag(button_8, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_8 = lv_label_create(button_8);
    lv_obj_set_align(label_8, LV_ALIGN_CENTER);
    lv_label_set_text(label_8, "Panel 8");

    lv_obj_t * button_9 = lv_button_create(container);
    lv_obj_set_size(button_9, 80, lv_pct(100));
    lv_obj_set_flag(button_9, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
    lv_obj_t * label_9 = lv_label_create(button_9);
    lv_obj_set_align(label_9, LV_ALIGN_CENTER);
    lv_label_set_text(label_9, "Panel 9");
}
#endif
