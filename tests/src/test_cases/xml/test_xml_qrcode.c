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

void test_xml_qrcode_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * qrcode_attrs[] = {
        "size", "150",
        "dark_color", "0x000000",
        "light_color", "0xFFFFFF",
        "data", "https://lvgl.io",
        NULL, NULL,
    };

    lv_obj_t * qrcode = lv_xml_create(scr, "lv_qrcode", qrcode_attrs);
    lv_obj_center(qrcode);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_qrcode.png");
}

#endif
