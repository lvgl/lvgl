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

void test_xml_tabview_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * dropdown_attrs[] = {
        "options", "First\nSecond\nThird",
        "text", "Select",
        "selected", "1",
        "style_bg_color", "0xaaaaff",
        NULL, NULL,
    };

    lv_obj_t * dropdown = lv_xml_create(scr, "lv_dropdown", dropdown_attrs);
    lv_obj_center(dropdown);
    lv_dropdown_open(dropdown);

    const char * list_attrs[] = {
        "style_bg_color", "0xffaaaa",
        NULL, NULL,
    };
    lv_xml_create(dropdown, "lv_dropdown-list", list_attrs);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_dropdown.png");
}

#endif
