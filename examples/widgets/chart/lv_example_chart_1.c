#include "../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Basic line chart with two series
 * @brief Line chart plotting one primary and one secondary Y-axis series with point drop shadows.
 *
 * A 200x150 chart is centered on the active screen with `LV_CHART_TYPE_LINE`.
 * Two series are added: a green series bound to `LV_CHART_AXIS_PRIMARY_Y`
 * filled with `lv_chart_set_next_value`, and a red series bound to
 * `LV_CHART_AXIS_SECONDARY_Y` filled by writing directly into the Y array.
 * Drop shadow styles on `LV_PART_ITEMS` give each point a glow and
 * `lv_chart_refresh` commits the direct writes.
 */
void lv_example_chart_1(void)
{
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_SECONDARY_Y);
    int32_t * ser2_y_points = lv_chart_get_series_y_array(chart, ser2);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        /*Set the next points on 'ser1'*/
        lv_chart_set_next_value(chart, ser1, (int32_t)lv_rand(10, 50));

        /*Directly set points on 'ser2'*/
        ser2_y_points[i] = (int32_t)lv_rand(50, 90);
    }

    lv_obj_set_style_drop_shadow_opa(chart, 255, LV_PART_ITEMS);
    lv_obj_set_style_drop_shadow_offset_x(chart, 0, LV_PART_ITEMS);
    lv_obj_set_style_drop_shadow_radius(chart, 20, LV_PART_ITEMS);
    /* Drop shadow color follows each series color automatically,
     * so no manual override is needed. */
    lv_chart_refresh(chart); /*Required after direct set*/
}

#endif
