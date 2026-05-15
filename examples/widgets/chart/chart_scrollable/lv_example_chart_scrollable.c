/**
 * @file lv_example_chart_scrollable.c
 */

#include "../../../../lvgl.h"

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
void lv_example_chart_scrollable_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Drag horizontally to scroll the chart; the scale below follows because both share the scrolling wrapper. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Chart: scrollable");

    /* Viewport: small fixed width; clips its scrollable child. */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 220, 180);
    lv_obj_set_style_pad_all(lv_obj_1, 0, 0);
    /* Wrapper: wider than the viewport so it scrolls. Hosts both chart and scale stacked vertically. */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_size(lv_obj_2, lv_pct(300), lv_pct(100));
    lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(lv_obj_2, 0, 0);
    lv_obj_set_style_border_width(lv_obj_2, 0, 0);
    lv_obj_set_style_pad_all(lv_obj_2, 4, 0);
    lv_obj_t * lv_chart_0 = lv_chart_create(lv_obj_2);
    lv_obj_set_width(lv_chart_0, lv_pct(100));
    lv_obj_set_flex_grow(lv_chart_0, 1);
    lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(lv_chart_0, 12);
    lv_chart_set_hor_div_line_count(lv_chart_0, 3);
    lv_chart_set_ver_div_line_count(lv_chart_0, 0);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0x6366f1),
                                                                LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t lv_chart_0_values_0[] = {32, 41, 38, 55, 62, 70, 65, 58, 47, 60, 75, 80};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 12);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 100);

    lv_obj_t * lv_scale_0 = lv_scale_create(lv_obj_2);
    lv_obj_set_size(lv_scale_0, lv_pct(100), 22);
    lv_scale_set_mode(lv_scale_0, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(lv_scale_0, 12);
    lv_scale_set_major_tick_every(lv_scale_0, 1);
    lv_scale_set_label_show(lv_scale_0, true);
    static const char * lv_scale_0_text_src_1[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};
    lv_scale_set_text_src(lv_scale_0, lv_scale_0_text_src_1);
}

