/**
 * @file lv_example_chart_types.c
 */

#include "../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Chart types
 * @brief Same data rendered as bar and scatter to highlight the type difference.
 *
 * The `type` attribute controls how a chart draws its series. `bar` paints one
 * column per slot; `scatter` plots (X, Y) pairs as dots. A scatter series binds
 * to *both* a Y axis and an X axis — that's what the OR-flavoured
 * `axis="primary_y|primary_x"` token encodes. Compare against
 * `chart_basic` (a `line` chart) to complete the family.
 */
void lv_example_chart_types(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* BAR chart: one column per slot */
    lv_obj_t * chart_1 = lv_chart_create(screen);
    lv_obj_set_size(chart_1, lv_pct(90), lv_pct(40));
    lv_chart_set_type(chart_1, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_1, 8);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(chart_1, lv_color_hex(0x6366f1), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t chart_1_values_0[] = {10, 35, 25, 55, 40, 70, 60, 85};
    lv_chart_set_series_values(chart_1, lv_chart_series_0, chart_1_values_0, 8);
    lv_chart_set_axis_min_value(chart_1, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(chart_1, LV_CHART_AXIS_PRIMARY_Y, 100);

    lv_obj_t * chart_2 = lv_chart_create(screen);
    lv_obj_set_size(chart_2, lv_pct(90), lv_pct(40));
    lv_chart_set_type(chart_2, LV_CHART_TYPE_STACKED);
    lv_chart_set_point_count(chart_2, 12);
    lv_chart_series_t * lv_chart_series_1 = lv_chart_add_series(chart_2, lv_color_hex(0xef4444),
                                                                (lv_chart_axis_t)(LV_CHART_AXIS_PRIMARY_Y | LV_CHART_AXIS_PRIMARY_X));
    static const int32_t chart_2_values_1[] = {8, 10, 40, 20, 80, 50, 20, 30, 60, 30, 40, 20};
    lv_chart_set_series_values(chart_2, lv_chart_series_1, chart_2_values_1, 12);
    lv_chart_series_t * lv_chart_series_2 = lv_chart_add_series(chart_2, lv_color_hex(0x22c55e),
                                                                (lv_chart_axis_t)(LV_CHART_AXIS_PRIMARY_Y | LV_CHART_AXIS_PRIMARY_X));
    static const int32_t chart_2_values_2[] = {10, 40, 20, 80, 50, 30, 40, 80, 40, 70, 80, 70};
    lv_chart_set_series_values(chart_2, lv_chart_series_2, chart_2_values_2, 12);
    lv_chart_set_axis_min_value(chart_2, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(chart_2, LV_CHART_AXIS_PRIMARY_Y, 150);
}
#endif
