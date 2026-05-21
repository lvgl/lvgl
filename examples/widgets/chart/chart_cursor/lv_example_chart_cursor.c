/**
 * @file lv_example_chart_cursor.c
 */

#include "../../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

/**
 * @title Chart cursor
 * @brief A cross-hair cursor pinned to a specific (X, Y) location on the chart.
 *
 * `<lv_chart-cursor>` adds a cursor overlay to a chart. `dir` chooses which
 * lines extend from the cursor point (`all` draws a full cross-hair; `hor`,
 * `ver`, `left`, `right`, `top`, `bottom` for partial guides). `pos_x`/`pos_y`
 * place it in chart value coordinates, so a position of `60, 70` lines up
 * with the 60th X step and the 70-on-the-Y-axis grid line. A typical use is
 * highlighting "today" on a time-series, or marking a threshold without
 * baking the line into the series.
 */
void lv_example_chart_cursor(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tweak `pos_x`/`pos_y` or `dir` to see how the cross-hair lines hug the cursor point. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Chart: cursor");

    lv_obj_t * lv_chart_0 = lv_chart_create(screen);
    lv_obj_set_size(lv_chart_0, lv_pct(90), 180);
    lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(lv_chart_0, 8);
    lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0x6366f1),
                                                                LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t lv_chart_0_values_0[] = {10, 35, 25, 55, 40, 70, 60, 85};
    lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 8);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 100);
    lv_chart_cursor_t * lv_chart_cursor_0 = lv_chart_add_cursor(lv_chart_0, lv_color_hex(0xef4444), LV_DIR_HOR);
    lv_chart_set_cursor_pos_x(lv_chart_0, lv_chart_cursor_0, 60);
    lv_chart_set_cursor_pos_y(lv_chart_0, lv_chart_cursor_0, 70);
}
#endif
