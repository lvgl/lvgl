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

void test_xml_roller_with_attrs(void)
{

    lv_obj_t * scr = lv_screen_active();

    const char * roller_attrs[] = {
        "width", "200",
        "options", "'a\nb\nc\nd\ne' infinite",
        "selected", "2 true",
        "visible_line_count", "3",
        NULL, NULL,
    };

    lv_obj_t * roller = lv_xml_create(scr, "lv_roller", roller_attrs);
    lv_obj_center(roller);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_roller.png");
}

#endif
