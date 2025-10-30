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

void test_xml_spinner_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * spinner_attrs[] = {
        "anim_duration", "1500",
        "arc_sweep", "270",
        NULL, NULL,
    };

    lv_obj_t * spinner = lv_xml_create(scr, "lv_spinner", spinner_attrs);
    lv_obj_center(spinner);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_spinner_1.png");

    lv_test_wait(750);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_spinner_2.png");

    lv_test_wait(750);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_spinner_1.png");
}

#endif
