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

static void test_with_attrs(const char * name)
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_xml_create(scr, name, NULL);

    const char * attrs_1[] = {
        "value", "30",
        "width", "100",
        NULL, NULL,
    };

    lv_xml_create(scr, name, attrs_1);

    const char * attrs_2[] = {
        "range", "-100 100",
        "mode", "symmetrical",
        "value", "50",
        NULL, NULL,
    };

    lv_xml_create(scr, name, attrs_2);

    const char * attrs_3[] = {
        "orientation", "vertical",
        "height", "80",
        "width", "30",
        "value", "40",
        NULL, NULL,
    };

    lv_xml_create(scr, name, attrs_3);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_bar.png");
}

void test_xml_bar_widget(void)
{
    test_with_attrs("lv_bar");
}

void test_xml_bar_component(void)
{
    const char * xml = "<component>"
                       "<view extends=\"lv_bar\">"
                       "</view>"
                       "</component>";

    lv_xml_component_register_from_data("bar_test", xml);

    test_with_attrs("bar_test");
}

#endif
