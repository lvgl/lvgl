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

void test_xml_label_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * textarea1_attrs[] = {
        "text", "This is the text with ellipses added automatically",
        "long_mode", "dots",
        "width", "100",
        "height", "40",
        "align", "center",
        "style_bg_opa", "50%",
        "style_bg_color", "0xf00",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", textarea1_attrs);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_label.png");
}

#endif
