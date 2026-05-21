/**
 * @file lv_example_scroll_one.c
 */

#include "../../lv_examples.h"
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
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 290, 150);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(lv_obj_1, 10, 0);
    lv_obj_set_scroll_snap_x(lv_obj_1, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLL_ONE, true);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_0, 80, lv_pct(100));
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "Panel 1");

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_1, 80, lv_pct(100));
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Panel 2");

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_2, 80, lv_pct(100));
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Panel 3");

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_3, 80, lv_pct(100));
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_3);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "Panel 4");

    lv_obj_t * lv_button_4 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_4, 80, lv_pct(100));
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_4);
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_4, "Panel 5");

    lv_obj_t * lv_button_5 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_5, 80, lv_pct(100));
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_5);
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_5, "Panel 6");

    lv_obj_t * lv_button_6 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_6, 80, lv_pct(100));
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_6);
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_6, "Panel 7");

    lv_obj_t * lv_button_7 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_7, 80, lv_pct(100));
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_7);
    lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_7, "Panel 8");

    lv_obj_t * lv_button_8 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_8, 80, lv_pct(100));
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_8);
    lv_obj_set_align(lv_label_8, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_8, "Panel 9");
}
#endif
