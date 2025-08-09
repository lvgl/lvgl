
#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_xml_chart_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * chart_attrs[] = {
        "width", "200",
        "height", "100",
        "type", "bar",
        "point_count", "8",
        "style_length:indicator", "10",
        "style_transform_rotation:indicator", "300",
        NULL, NULL,
    };

    lv_obj_t * chart = lv_xml_create(scr, "lv_chart", chart_attrs);
    lv_obj_center(chart);

    const char * primary_y_axis_attrs[] = {
        "axis", "primary_y",
        "min_value", "0",
        "max_value", "40",
        NULL, NULL,
    };
    lv_xml_create(chart, "lv_chart-axis", primary_y_axis_attrs);

    const char * series_1_axis_attrs[] = {
        "axis", "primary_y",
        "color", "0xff0000",
        "values", "10 20 30 10 20 30",
        NULL, NULL,
    };

    lv_chart_series_t * ser1 = lv_xml_create(chart, "lv_chart-series", series_1_axis_attrs);

    const char * series_2_axis_attrs[] = {
        "axis", "secondary_y",
        "color", "0x00ff00",
        "values", "90 80",
        NULL, NULL,
    };

    lv_chart_series_t * ser2 = lv_xml_create(chart, "lv_chart-series", series_2_axis_attrs);

    const char * secondary_y_axis_attrs[] = {
        "axis", "secondary_y",
        "min_value", "70",
        "max_value", "90",
        NULL, NULL,
    };
    lv_xml_create(chart, "lv_chart-axis", secondary_y_axis_attrs);

    lv_chart_set_next_value(chart, ser1, 40);
    lv_chart_set_next_value(chart, ser2, 70);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_chart.png");
}

#endif
