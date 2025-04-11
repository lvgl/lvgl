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

void test_arc_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * attrs_1[] = {
        "bg_angles", "0 100",
        "range", "-20 20",
        "value", "10",
        "mode", "reverse",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_arc", attrs_1);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_arc.png");
}

#endif
