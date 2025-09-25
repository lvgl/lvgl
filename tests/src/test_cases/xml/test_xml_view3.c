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

void test_xml_view3_scoping(void)
{
    lv_xml_register_component_from_file("A:src/test_assets/xml/globals.xml");
    lv_xml_register_component_from_file("A:src/test_assets/xml/view3.xml");

    lv_xml_create(lv_screen_active(), "view3", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/view3.png");

    /*Fonts and image defined in globals.xml*/
    TEST_ASSERT_NOT_EQUAL(lv_font_get_default(), lv_xml_get_font(NULL, "my_bin_font"));
    TEST_ASSERT_NOT_EQUAL(lv_font_get_default(), lv_xml_get_font(NULL, "noto_32"));
    TEST_ASSERT_EQUAL_STRING("A:src/test_assets/test_img_lvgl_logo.png", lv_xml_get_image(NULL, "logo"));
}

#endif
