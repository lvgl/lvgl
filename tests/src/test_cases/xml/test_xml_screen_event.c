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

void test_xml_screen_event_1(void)
{
    const char * screen1_xml = {
        "  <screen>"
        "	  <view style_bg_color=\"0xff7788\" name=\"first\">"
        "		  <lv_button>"
        "			  <lv_label text=\"Create\"/>"
        "			  <screen_create_event screen=\"screen2\" anim_type=\"over_right\" duration=\"500\" delay=\"1000\"/>"
        "		  </lv_button>"
        "	  </view>"
        "  </screen>"
    };

    const char * screen2_xml = {
        "  <screen>"
        "	  <view style_bg_color=\"0x77ff88\" name=\"second\">"
        "		  <lv_button>"
        "			  <lv_label text=\"Load\"/>"
        "			  <screen_load_event screen=\"first\"/>"
        "		  </lv_button>"
        "	  </view>"
        "  </screen>"
    };


    lv_xml_component_register_from_data("screen1", screen1_xml);
    lv_xml_component_register_from_data("screen2", screen2_xml);

    lv_obj_t * default_screen = lv_screen_active();

    lv_obj_t * screen1;
    screen1 = lv_xml_create(NULL, "screen1", NULL);
    TEST_ASSERT_NOT_NULL(lv_display_get_screen_by_name(NULL, "first"));

    lv_screen_load(screen1);

    lv_test_mouse_click_at(10, 10);
    lv_test_wait(200);
    /*There is a delay so stays on the first screen*/
    TEST_ASSERT_EQUAL_STRING("first", lv_obj_get_name(lv_screen_active()));

    /*second should be created*/
    TEST_ASSERT_NOT_NULL(lv_display_get_screen_by_name(NULL, "second"));

    lv_test_wait(1300);
    TEST_ASSERT_EQUAL_STRING("second", lv_obj_get_name(lv_screen_active()));

    /*No delay, go back the first screen immediately*/
    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_EQUAL_STRING("first", lv_obj_get_name(lv_screen_active()));
    /*second should be deleted*/
    TEST_ASSERT_NULL(lv_display_get_screen_by_name(NULL, "second"));

    /*Clean up*/
    lv_screen_load(default_screen);
    lv_obj_delete(screen1);
    TEST_ASSERT_NULL(lv_display_get_screen_by_name(NULL, "first"));
}

#endif
