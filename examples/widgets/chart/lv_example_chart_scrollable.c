/**
 * @file lv_example_chart_scrollable.c
 */

#include "../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Scrollable chart
 * @brief A chart wider than its container scrolls horizontally as the user drags it.
 *
 * LVGL doesn't expose a chart-specific zoom: to let a chart grow beyond the
 * visible width, wrap it in a parent that *is* the viewport, then size the
 * chart's width to a percentage larger than 100%. The parent's default
 * `scrollable` behaviour kicks in. A horizontal `lv_scale` sits inside the
 * same wrapper so its labels scroll in lockstep with the bars.
 */
void lv_example_chart_scrollable(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Viewport: small fixed width; clips its scrollable child. */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, 300, 180);
    lv_obj_set_style_pad_all(container_1, 0, 0);
    /* Wrapper: wider than the viewport so it scrolls. Hosts both chart and scale stacked vertically. */
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_size(container_2, lv_pct(200), lv_pct(100));
    lv_obj_set_flex_flow(container_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(container_2, 0, 0);
    lv_obj_set_style_border_width(container_2, 0, 0);
    lv_obj_set_style_pad_all(container_2, 4, 0);
    lv_obj_t * chart = lv_chart_create(container_2);
    lv_obj_set_width(chart, lv_pct(100));
    lv_obj_set_flex_grow(chart, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, 12);
    lv_chart_set_hor_div_line_count(chart, 3);
    lv_chart_set_ver_div_line_count(chart, 0);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(chart, lv_color_hex(0x6366f1), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t chart_values_0[] = {32, 41, 38, 55, 62, 70, 65, 58, 47, 60, 75, 80};
    lv_chart_set_series_values(chart, lv_chart_series_0, chart_values_0, 12);
    lv_chart_set_axis_min_value(chart, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(chart, LV_CHART_AXIS_PRIMARY_Y, 100);

    lv_obj_t * scale = lv_scale_create(container_2);
    lv_obj_set_width(scale, lv_pct(100));
    lv_obj_set_style_pad_hor(scale, 30, 0);
    lv_obj_set_height(scale, 22);
    lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale, 12);
    lv_scale_set_major_tick_every(scale, 1);
    lv_scale_set_label_show(scale, true);
    static const char * scale_text_src_1[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};
    lv_scale_set_text_src(scale, scale_text_src_1);
}
#endif
