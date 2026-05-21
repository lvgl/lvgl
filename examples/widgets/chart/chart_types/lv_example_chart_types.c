/**
 * @file lv_example_chart_types.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Swap `type` between `bar`, `line`, and `scatter` to see how the same series renders. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Chart: types (bar + scatter)");

    /* BAR chart: one column per slot */
    lv_obj_t * lv_chart_0 = lv_chart_create(screen);
    lv_obj_set_size(lv_chart_0, lv_pct(90), 120);
    lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(lv_chart_0, 8);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0x6366f1),
                                                                LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t lv_chart_0_values_0[] = {10, 35, 25, 55, 40, 70, 60, 85};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 8);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 100);

    lv_obj_t * lv_chart_1 = lv_chart_create(screen);
    lv_obj_set_size(lv_chart_1, lv_pct(90), 120);
    lv_chart_set_type(lv_chart_1, LV_CHART_TYPE_SCATTER);
    lv_chart_set_point_count(lv_chart_1, 6);
    lv_chart_series_t * lv_chart_series_1 = lv_chart_add_series(lv_chart_1, lv_color_hex(0xef4444),
                                                                (lv_chart_axis_t)(LV_CHART_AXIS_PRIMARY_Y | LV_CHART_AXIS_PRIMARY_X));
    static const int32_t lv_chart_1_values_1[] = {10, 40, 20, 80, 50, 30};
    lv_chart_set_series_values(lv_chart_1, lv_chart_series_1, lv_chart_1_values_1, 6);
    lv_chart_set_axis_min_value(lv_chart_1, LV_CHART_AXIS_PRIMARY_X, 0);
    lv_chart_set_axis_max_value(lv_chart_1, LV_CHART_AXIS_PRIMARY_X, 100);
    lv_chart_set_axis_min_value(lv_chart_1, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_1, LV_CHART_AXIS_PRIMARY_Y, 100);
}
#endif
