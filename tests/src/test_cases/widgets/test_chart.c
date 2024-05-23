#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * chart = NULL;

static lv_color_t red_color;

void setUp(void)
{
    active_screen = lv_screen_active();
    chart = lv_chart_create(active_screen);

    red_color = lv_palette_main(LV_PALETTE_RED);
}

void tearDown(void)
{
    /* Is there a way to destroy a chart without having to call remove_series for each of it series? */
    lv_obj_clean(active_screen);
}

/* NOTE: Default chart type is LV_CHART_TYPE_LINE */
void test_chart_add_series(void)
{
    lv_chart_series_t * red_series;

    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);

    TEST_ASSERT_NOT_NULL_MESSAGE(red_series, "Red series not added to chart");
    TEST_ASSERT_NULL_MESSAGE(red_series->x_points, "X points in non scatter chart should not point to anything");

    lv_chart_remove_series(chart, red_series);
}

void test_chart_set_point_count_increments(void)
{
    lv_chart_series_t * red_series;
    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);

    uint16_t points_in_series = lv_chart_get_point_count(chart);
    uint16_t new_point_count = points_in_series * 2;
    lv_chart_set_point_count(chart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_chart_get_point_count(chart),
                              "Actual points in chart are less than expected");
    lv_chart_remove_series(chart, red_series);
}

void test_chart_set_point_count_decrements(void)
{
    lv_chart_series_t * red_series;
    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);
    uint16_t points_in_series = lv_chart_get_point_count(chart);
    uint16_t new_point_count = points_in_series / 2;

    lv_chart_set_point_count(chart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_chart_get_point_count(chart),
                              "Actual points in chart are more than expected");
    lv_chart_remove_series(chart, red_series);
}

void test_chart_set_point_count_as_same(void)
{
    lv_chart_series_t * red_series;
    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);
    uint16_t points_in_series = lv_chart_get_point_count(chart);
    uint16_t new_point_count = points_in_series;

    lv_chart_set_point_count(chart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_chart_get_point_count(chart),
                              "Actual points is not equal to original point count");
    lv_chart_remove_series(chart, red_series);
}

void test_chart_set_new_point_count_as_zero(void)
{
    lv_chart_series_t * red_series;
    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);

    lv_chart_set_point_count(chart, 0u);

    TEST_ASSERT_EQUAL_MESSAGE(1u, lv_chart_get_point_count(chart), "Actual points in chart are more than 1");
    lv_chart_remove_series(chart, red_series);
}

void test_chart_point_is_added_at_the_end_of_a_series(void)
{
    lv_chart_series_t * red_series;
    red_series = lv_chart_add_series(chart, red_color, LV_CHART_AXIS_SECONDARY_Y);

    TEST_ASSERT_NOT_NULL_MESSAGE(red_series, "Red series not added to chart");
    TEST_ASSERT_NULL_MESSAGE(red_series->x_points, "X points in non scatter chart should not point to anything");

    lv_chart_remove_series(chart, red_series);
}

void test_chart_one_point_when_setting_point_count_to_zero(void)
{
    lv_chart_set_point_count(chart, 0u);
    TEST_ASSERT_EQUAL(1u, lv_chart_get_point_count(chart));
}

static void chart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_DRAW_TASK_ADDED) {
        lv_draw_task_t * draw_task = lv_event_get_param(e);
        lv_draw_dsc_base_t * base_dsc = draw_task->draw_dsc;

        lv_chart_type_t chart_type = lv_chart_get_type(obj);
        if(chart_type == LV_CHART_TYPE_LINE && base_dsc->part != LV_PART_INDICATOR)  return;
        if(chart_type == LV_CHART_TYPE_BAR && base_dsc->part != LV_PART_ITEMS)  return;

        const lv_chart_series_t * ser = NULL;
        for(uint32_t i = 0; i < base_dsc->id1 + 1; i++) {
            ser = lv_chart_get_series_next(obj, ser);
        }

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, ser->y_points[base_dsc->id2]);

        lv_point_t text_size;
        lv_text_get_size(&text_size, buf, LV_FONT_DEFAULT, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_task->area.x1 + lv_area_get_width(&draw_task->area) / 2 - text_size.x / 2;
        txt_area.x2 = txt_area.x1 + text_size.x;
        txt_area.y2 = draw_task->area.y1 - LV_DPX(15);
        txt_area.y1 = txt_area.y2 - text_size.y;

        lv_area_t bg_area;
        bg_area.x1 = txt_area.x1 - LV_DPX(8);
        bg_area.x2 = txt_area.x2 + LV_DPX(8);
        bg_area.y1 = txt_area.y1 - LV_DPX(8);
        bg_area.y2 = txt_area.y2 + LV_DPX(8);

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = ser->color;
        rect_dsc.radius = LV_DPX(5);
        lv_draw_rect(base_dsc->layer, &rect_dsc, &bg_area);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_white();
        label_dsc.font = LV_FONT_DEFAULT;
        label_dsc.text = buf;
        label_dsc.text_local = true;
        lv_draw_label(base_dsc->layer, &label_dsc, &txt_area);
    }
}

void test_draw_task_hooking(void)
{
    lv_obj_t * chart_wrapper = lv_obj_create(lv_screen_active());
    lv_obj_set_size(chart_wrapper, 600, 300);
    lv_obj_center(chart_wrapper);

    lv_obj_set_parent(chart, chart_wrapper);
    lv_obj_set_size(chart, lv_pct(200), lv_pct(100));
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_chart_set_div_line_count(chart, 0, 21);
    lv_chart_set_point_count(chart, 21);
    lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_chart_series_t * chart_set1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_chart_series_t * chart_set2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_chart_series_t * chart_set3 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), 0);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 700);

    int32_t points[21] = {0, 31, 59, 81, 95, 100, 95, 81, 59, 31, 0, -31, -59, -81, -95, -100, -95, -81, -59, -31, 0};

    for(uint32_t i = 0; i < 21; i++) {
        lv_chart_set_next_value(chart, chart_set1, points[i] + 100);
        lv_chart_set_next_value(chart, chart_set2, points[i] + 300);
        lv_chart_set_next_value(chart, chart_set3, points[i] + 500);
    }

    lv_obj_scroll_to_x(chart_wrapper, 300, LV_ANIM_OFF);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/chart_line_draw_hook.png");

    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/chart_bar_draw_hook.png");
}

#endif
