/**
 * @file lv_example_chart_series.c
 */

#include "../../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Chart data series
 * @brief Two series on the same chart, each bound to its own Y axis.
 *
 * Two `<lv_chart-series>` children sit on one chart but reference different
 * axes via `axis="primary_y"` and `axis="secondary_y"`. Each axis gets its
 * own `<lv_chart-axis>` range, so a 0..100 temperature series and a 0..2000
 * pressure series share the plot without one squashing the other. The series
 * colour matches the conceptual reading, not the axis.
 */
void lv_example_chart_series(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit either axis range; the series tied to that axis rescales while the other stays put. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Chart: dual-axis series");

    lv_obj_t * lv_chart_0 = lv_chart_create(screen);
    lv_obj_set_size(lv_chart_0, lv_pct(90), 180);
    lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(lv_chart_0, 8);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0xef4444),
                                                                LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t lv_chart_0_values_0[] = {22, 28, 35, 50, 65, 70, 60, 45};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 8);
    lv_chart_series_t * lv_chart_series_1 = lv_chart_add_series(lv_chart_0, lv_color_hex(0x6366f1),
                                                                LV_CHART_AXIS_SECONDARY_Y);
    static const int32_t lv_chart_0_values_1[] = {980, 1100, 1400, 1600, 1750, 1850, 1900, 1700};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_1, lv_chart_0_values_1, 8);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 100);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_SECONDARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_SECONDARY_Y, 2000);
}
#endif
