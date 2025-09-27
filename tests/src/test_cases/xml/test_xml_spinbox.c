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

void test_spinbox_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * attrs_1[] = {
        "min_value", "0",
        "max_value", "3000",
        "digit_count", "5",
        "dec_point_pos", "3",
        "value", "1234",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_spinbox", attrs_1);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_spinbox.png");
}

#endif
