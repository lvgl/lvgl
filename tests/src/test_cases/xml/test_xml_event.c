#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include <stdlib.h>

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static int32_t cnt;
static void count_event_cb(lv_event_t * e)
{
    const char * user_data = lv_event_get_user_data(e);
    int inc =  atoi(user_data);

    cnt += inc;
}

void test_xml_event_call_function_attr(void)
{

    lv_xml_register_event_cb(NULL, "count_cb", count_event_cb);

    lv_obj_t * scr = lv_screen_active();

    const char * button_attrs[] = {
        "width", "300",
        "x", "10",
        "y", "10",
        NULL, NULL,
    };

    lv_obj_t * button = lv_xml_create(scr, "lv_button", button_attrs);

    const char * event_attrs[] = {
        "callback", "count_cb",
        "user_data", "3",
        "trigger", "clicked",
        NULL, NULL,
    };

    lv_xml_create(button, "event_cb", event_attrs);


    const char * label_attrs[] = {
        "text", "Click me!",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(button, "lv_label", label_attrs);

    lv_refr_now(NULL);  /*Make sure that the coordinates are calculated*/

    cnt = 0;
    lv_test_mouse_click_at(30, 20);
    TEST_ASSERT_EQUAL(3, cnt);

    lv_test_wait(100);
    lv_test_mouse_click_at(30, 20);
    TEST_ASSERT_EQUAL(6, cnt);
}

void test_xml_event_call_function_component(void)
{
    const char * xml = {
        "<component>"
        "   <view extends=\"lv_button\" x=\"5\" y=\"5\">"
        "		<lv_label text=\"Click me\"/>"
        "		<lv_obj-event_cb trigger=\"clicked\" callback=\"count_cb\" user_data=\"3\"/>"
        "	</view>"
        "</component>"
    };

    lv_xml_register_event_cb(NULL, "count_cb", count_event_cb);
    lv_xml_component_register_from_data("my_button", xml);
    lv_xml_create(lv_screen_active(), "my_button", NULL);

    lv_refr_now(NULL);  /*Make sure that the coordinates are calculated*/

    cnt = 0;
    lv_test_mouse_click_at(30, 10);
    TEST_ASSERT_EQUAL(3, cnt);

    lv_test_wait(100);
    lv_test_mouse_click_at(30, 10);
    TEST_ASSERT_EQUAL(6, cnt);
}
#endif
