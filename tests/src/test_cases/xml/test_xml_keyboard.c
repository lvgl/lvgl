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

void test_xml_image_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * keyboard1_attrs[] = {
        "mode", "text_upper",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_keyboard", keyboard1_attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_keyboard.png");
}

#endif
