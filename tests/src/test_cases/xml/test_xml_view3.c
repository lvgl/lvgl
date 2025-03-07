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
    lv_xml_component_register_from_file("A:src/test_assets/xml/globals.xml");
    lv_xml_component_register_from_file("A:src/test_assets/xml/view3.xml");

    lv_xml_create(lv_screen_active(), "view3", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/view3.png");
}

#endif
