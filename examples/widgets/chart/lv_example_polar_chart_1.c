#include "../../lv_examples.h"
#if LV_USE_POLAR_CHART && LV_BUILD_EXAMPLES

/**
 * A very basic line polar chart
 */
void lv_example_polar_chart_1(void)
{
    /*Create a polar chart*/
    lv_obj_t * chart;
    chart = lv_polar_chart_create(lv_screen_active());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_polar_chart_set_type(chart, LV_POLAR_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_polar_chart_series_t * ser1 = lv_polar_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_POLAR_CHART_AXIS_RADIAL);
    lv_polar_chart_series_t * ser2 = lv_polar_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_POLAR_CHART_AXIS_RADIAL);
    int32_t * ser2_y_points = lv_polar_chart_get_series_radial_array(chart, ser2);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        /*Set the next points on 'ser1'*/
        lv_polar_chart_set_next_value(chart, ser1, (int32_t)lv_rand(10, 50));

        /*Directly set points on 'ser2'*/
        ser2_y_points[i] = (int32_t)lv_rand(50, 90);
    }

    lv_polar_chart_refresh(chart); /*Required after direct set*/
}

#endif
