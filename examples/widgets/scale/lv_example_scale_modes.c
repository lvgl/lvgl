/**
 * @file lv_example_scale_modes.c
 */

#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale linear modes
 * @brief Horizontal and vertical scales, ticks on either side of the rail.
 *
 * Linear scales come in four flavours: `horizontal_top` / `horizontal_bottom`
 * lay the rail across and put ticks above or below it; `vertical_left` /
 * `vertical_right` rotate the same idea 90°. The four scales below share the
 * same tick count and value range so the only visible difference is layout.
 */
void lv_example_scale_modes(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Swap `mode` between the four `horizontal_*`/`vertical_*` values to compare layouts. */
    lv_obj_t * scale_1 = lv_scale_create(screen);
    lv_obj_set_size(scale_1, 120, 30);
    lv_scale_set_mode(scale_1, LV_SCALE_MODE_HORIZONTAL_TOP);
    lv_scale_set_total_tick_count(scale_1, 11);
    lv_scale_set_major_tick_every(scale_1, 2);
    lv_scale_set_label_show(scale_1, true);
    lv_scale_set_min_value(scale_1, 0);
    lv_scale_set_max_value(scale_1, 100);
    lv_obj_set_align(scale_1, LV_ALIGN_LEFT_MID);
    lv_obj_set_y(scale_1, 20);
    lv_obj_set_x(scale_1, 20);

    lv_obj_t * scale_2 = lv_scale_create(screen);
    lv_obj_set_size(scale_2, 120, 30);
    lv_scale_set_mode(scale_2, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_2, 11);
    lv_scale_set_major_tick_every(scale_2, 2);
    lv_scale_set_label_show(scale_2, true);
    lv_scale_set_min_value(scale_2, 0);
    lv_scale_set_max_value(scale_2, 100);
    lv_obj_set_align(scale_2, LV_ALIGN_LEFT_MID);
    lv_obj_set_y(scale_2, -20);
    lv_obj_set_x(scale_2, 20);

    lv_obj_t * scale_3 = lv_scale_create(screen);
    lv_obj_set_size(scale_3, 120, 120);
    lv_scale_set_mode(scale_3, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_total_tick_count(scale_3, 11);
    lv_scale_set_major_tick_every(scale_3, 2);
    lv_scale_set_label_show(scale_3, true);
    lv_scale_set_min_value(scale_3, 0);
    lv_scale_set_max_value(scale_3, 100);
    lv_obj_set_align(scale_3, LV_ALIGN_CENTER);

    lv_obj_t * scale_4 = lv_scale_create(screen);
    lv_obj_set_size(scale_4, 90, 90);
    lv_scale_set_mode(scale_4, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_total_tick_count(scale_4, 11);
    lv_scale_set_major_tick_every(scale_4, 2);
    lv_scale_set_label_show(scale_4, true);
    lv_scale_set_min_value(scale_4, 0);
    lv_scale_set_max_value(scale_4, 100);
    lv_obj_set_align(scale_4, LV_ALIGN_CENTER);

    lv_obj_t * scale_5 = lv_scale_create(screen);
    lv_obj_set_size(scale_5, 30, 160);
    lv_scale_set_mode(scale_5, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_scale_set_total_tick_count(scale_5, 11);
    lv_scale_set_major_tick_every(scale_5, 2);
    lv_scale_set_label_show(scale_5, true);
    lv_scale_set_min_value(scale_5, 0);
    lv_scale_set_max_value(scale_5, 100);
    lv_obj_set_align(scale_5, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(scale_5, -20);

    lv_obj_t * scale_6 = lv_scale_create(screen);
    lv_obj_set_size(scale_6, 30, 160);
    lv_scale_set_mode(scale_6, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_scale_set_total_tick_count(scale_6, 11);
    lv_scale_set_major_tick_every(scale_6, 2);
    lv_scale_set_label_show(scale_6, true);
    lv_scale_set_min_value(scale_6, 0);
    lv_scale_set_max_value(scale_6, 100);
    lv_obj_set_align(scale_6, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(scale_6, -60);
}
#endif
