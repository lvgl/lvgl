#include "../../lv_examples.h"
#if LV_USE_POLARCHART && LV_BUILD_EXAMPLES

/**
 * A very basic line polarchart
 */
void lv_example_polarchart_1(void)
{
    /*Create a polarchart*/
    lv_obj_t * polarchart;
    polarchart = lv_polarchart_create(lv_screen_active());
    lv_obj_set_size(polarchart, 200, 150);
    lv_obj_center(polarchart);
    lv_polarchart_set_type(polarchart, LV_POLARCHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_polarchart_series_t * ser1 = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_GREEN),
                                                               LV_POLARCHART_AXIS_RADIAL);
    lv_polarchart_series_t * ser2 = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_RED),
                                                               LV_POLARCHART_AXIS_RADIAL);
    int32_t * ser2_y_points = lv_polarchart_get_series_radial_array(polarchart, ser2);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        /*Set the next points on 'ser1'*/
        lv_polarchart_set_next_value(polarchart, ser1, (int32_t)lv_rand(10, 50));

        /*Directly set points on 'ser2'*/
        ser2_y_points[i] = (int32_t)lv_rand(50, 90);
    }

    lv_polarchart_refresh(chart); /*Required after direct set*/
}

#endif
