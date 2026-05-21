/**
 * @file lv_example_chart_ticks_labels.c
 */

#include "../../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Chart ticks and labels
 * @brief Pair a chart with an `lv_scale` to get a labelled X axis.
 *
 * A chart has no built-in tick or label rendering for its X axis — the
 * convention is to stack a horizontal `lv_scale` directly below, sized to the
 * chart's width and with `total_tick_count` matching the chart's `point_count`.
 * `text_src` is a space-separated list of single-quoted labels (one per major
 * tick), the same syntax button matrix maps use. For the Y axis you can place
 * a vertical scale on either side of the chart with the same technique.
 */
void lv_example_chart_ticks_labels(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* 💡 Edit the `text_src` list to relabel the X axis; entries must match `total_tick_count`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Chart: ticks and labels");

    lv_obj_t * lv_chart_0 = lv_chart_create(screen);
    lv_obj_set_size(lv_chart_0, lv_pct(90), 160);
    lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(lv_chart_0, 6);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0x6366f1),
                                                                LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t lv_chart_0_values_0[] = {20, 45, 30, 70, 55, 80};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 6);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 100);

    lv_obj_t * lv_scale_0 = lv_scale_create(screen);
    lv_obj_set_size(lv_scale_0, lv_pct(90), 28);
    lv_obj_set_style_pad_hor(lv_scale_0, 24, 0);
    lv_scale_set_mode(lv_scale_0, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(lv_scale_0, 6);
    lv_scale_set_major_tick_every(lv_scale_0, 1);
    static const char * lv_scale_0_text_src_1[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", NULL};
    lv_scale_set_text_src(lv_scale_0, lv_scale_0_text_src_1);
}
#endif
