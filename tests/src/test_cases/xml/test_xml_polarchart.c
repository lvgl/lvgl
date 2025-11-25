
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

void test_xml_polarchart_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * polarchart_attrs[] = {
        "width", "200",
        "height", "100",
        "type", "bar",
        "point_count", "8",
        "style_length:indicator", "10",
        "style_transform_rotation:indicator", "300",
        NULL, NULL,
    };

    lv_obj_t * chart = lv_xml_create(scr, "lv_chart", polarchart_attrs);
    lv_obj_center(chart);

    const char * radial_axis_attrs[] = {
        "axis", "radial",
        "min_value", "0",
        "max_value", "40",
        NULL, NULL,
    };
    lv_xml_create(chart, "lv_chart-axis", radial_axis_attrs);

    const char * series_1_axis_attrs[] = {
        "axis", "radial",
        "color", "0xff0000",
        "values", "10 20 30 10 20 30",
        NULL, NULL,
    };

    lv_polarchart_series_t * ser1 = lv_xml_create(chart, "lv_chart-series", series_1_axis_attrs);

    lv_polarchart_set_next_value(chart, ser1, 40);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_polarchart.png");
}

#endif
