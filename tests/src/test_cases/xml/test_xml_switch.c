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


void test_xml_switch_widget(void)
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_xml_create(scr, "lv_switch", NULL);

    const char * attrs_1[] = {
        "width", "100",
        "height", "40",
        "checked", "true",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_switch", attrs_1);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_switch.png");
}

#endif
