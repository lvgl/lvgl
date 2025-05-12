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

void test_xml_textarea_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * textarea1_attrs[] = {
        "text", "This is the text",
        "style_text_align", "center",
        "one_line", "true",
        "width", "400",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_textarea", textarea1_attrs);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_textarea.png");
}

#endif
