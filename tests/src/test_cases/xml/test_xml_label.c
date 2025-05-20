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

    const char * label1_attrs[] = {
        "text", "This is the text with ellipses added automatically",
        "long_mode", "dots",
        "width", "100",
        "height", "40",
        "align", "center",
        "style_bg_opa", "50%",
        "style_bg_color", "0xf00",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", label1_attrs);

    static lv_subject_t s1;
    lv_subject_init_int(&s1, 20);
    lv_xml_register_subject(NULL, "s1", &s1);

    const char * label2_attrs[] = {
        "bind_text", "s1 'We have %d users'",
        "y", "10",
        "x", "5",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", label2_attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_label.png");
}

#endif
