#include "../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * A very basic line chart
 */
void lv_example_chart_1(void)
{
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart, lv_pct(100), lv_pct(100));
    //    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_point_count(chart, 30);
    lv_obj_set_style_bg_opa(chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_opa(chart, 0, 0);

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    uint32_t i;
    for(i = 0; i < 30; i++) {
        /*Set the next points on 'ser1'*/
        lv_chart_set_next_value(chart, ser1, (int32_t)lv_rand(10, 90));

    }

    lv_chart_refresh(chart); /*Required after direct set*/
}

#endif
