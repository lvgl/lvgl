#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * chart = NULL;

static lv_color_t red_color;

void setUp(void)
{
    active_screen = lv_scr_act();
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

#endif
