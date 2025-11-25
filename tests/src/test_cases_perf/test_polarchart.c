#if LV_BUILD_TEST_PERF
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * polarchart = NULL;

static lv_color_t red_color;

void setUp(void)
{
    active_screen = lv_screen_active();
    polarchart = lv_polarchart_create(active_screen);

    red_color = lv_palette_main(LV_PALETTE_RED);
}

void test_polarchart(void)
{
    lv_polarchart_add_series(polarchart, red_color,
                        LV_CHART_AXIS_RADIAL);

    TEST_ASSERT_MAX_TIME(lv_polarchart_add_series, 1, polarchart, red_color,
                         LV_CHART_AXIS_RADIAL);

    for(size_t i = 0; i < 10; ++i) {
        uint16_t points_in_series = lv_polarchart_get_point_count(polarchart);
        uint16_t new_point_count = points_in_series * 2;

        TEST_ASSERT_MAX_TIME(lv_polarchart_set_point_count, 1, polarchart,
                             new_point_count);
    }
}
#endif
