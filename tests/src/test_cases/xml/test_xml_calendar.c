
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

void test_xml_calendar_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * calendar_attrs[] = {
        "width", "200",
        "height", "200",
        "x", "10",
        "y", "10",
        "today_date", "2025 05 06",
        "shown_month", "2025 05",
        NULL, NULL,
    };
    lv_obj_t * calendar;
    calendar = lv_xml_create(scr, "lv_calendar", calendar_attrs);
    lv_xml_create(calendar, "lv_calendar-header_arrow", NULL);

    calendar = lv_xml_create(scr, "lv_calendar", calendar_attrs);
    lv_obj_set_x(calendar, 250);
    lv_xml_create(calendar, "lv_calendar-header_dropdown", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_calendar.png");
}

#endif
