#if LV_BUILD_TEST_PERF
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

void test_chart(void)
{
    lv_chart_add_series(chart, red_color,
                        LV_CHART_AXIS_SECONDARY_Y);

    TEST_ASSERT_MAX_TIME(lv_chart_add_series, 1, chart, red_color,
                         LV_CHART_AXIS_SECONDARY_Y);

    for(size_t i = 0; i < 10; ++i) {
        uint16_t points_in_series = lv_chart_get_point_count(chart);
        uint16_t new_point_count = points_in_series * 2;

        TEST_ASSERT_MAX_TIME(lv_chart_set_point_count, 1, chart,
                             new_point_count);
    }
}
#endif
