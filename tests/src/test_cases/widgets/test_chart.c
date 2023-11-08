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

#endif
