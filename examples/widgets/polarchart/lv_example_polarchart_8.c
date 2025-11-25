#include "../../lv_examples.h"
#if LV_USE_POLARCHART && LV_DRAW_SW_COMPLEX && LV_BUILD_EXAMPLES

static void add_data(lv_timer_t * t)
{
    lv_obj_t * polarchart = (lv_obj_t *)lv_timer_get_user_data(t);
    lv_polarchart_series_t * ser = lv_polarchart_get_series_next(polarchart, NULL);

    lv_polarchart_set_next_value(polarchart, ser, (int32_t)lv_rand(10, 90));

    uint32_t p = lv_polarchart_get_point_count(polarchart);
    uint32_t s = lv_polarchart_get_angle_start_point(polarchart, ser);
    int32_t * a = lv_polarchart_get_series_radial_array(polarchart, ser);

    a[(s + 1) % p] = LV_POLARCHART_POINT_NONE;
    a[(s + 2) % p] = LV_POLARCHART_POINT_NONE;
    a[(s + 2) % p] = LV_POLARCHART_POINT_NONE;

    lv_polarchart_refresh(polarchart);
}

/**
 * Circular line polarchart with gap
 */
void lv_example_polarchart_8(void)
{
    /*Create a stacked_area_polarchart.obj*/
    lv_obj_t * polarchart = lv_polarchart_create(lv_screen_active());
    lv_polarchart_set_update_mode(polarchart, LV_POLARCHART_UPDATE_MODE_CIRCULAR);
    lv_obj_set_style_size(polarchart, 0, 0, LV_PART_INDICATOR);
    lv_obj_set_size(polarchart, 280, 150);
    lv_obj_center(polarchart);

    lv_polarchart_set_point_count(polarchart, 80);
    lv_polarchart_series_t * ser = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_RED), LV_POLARCHART_AXIS_RADIAL);
    /*Prefill with data*/
    uint32_t i;
    for(i = 0; i < 80; i++) {
        lv_polarchart_set_next_value(polarchart, ser, (int32_t)lv_rand(10, 90));
    }

    lv_timer_create(add_data, 300, polarchart);

}

#endif
