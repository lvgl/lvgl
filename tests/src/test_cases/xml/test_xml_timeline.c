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

void test_xml_complex(void)
{
    lv_xml_register_component_from_file("A:src/test_assets/xml/my_button_anim.xml");
    lv_xml_register_component_from_file("A:src/test_assets/xml/list_item_anim.xml");

    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "list_item_anim", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_1.png");

    lv_test_mouse_move_to_obj(lv_obj_get_child_by_name(obj, "my_button_anim_0"));
    /*Click Button 1 to trigger a load animation*/
    lv_test_mouse_click_at(45, 45);

    lv_test_wait(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_2.png");
    lv_test_wait(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_3.png");
    lv_test_wait(500);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_1.png"); /*Loaded, initial state*/

    /*Click Button 2 to trigger a grow/shrink animation*/
    lv_test_mouse_click_at(45, 90);
    lv_test_wait(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_4.png");
    lv_test_wait(300);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_5.png");

    /*1000 delay on the play_timeline_event trigger="released" in list_item_anim.xml
     *and 300 delay in the shrink animation in my_button_anim.xml.
     *But the clock started to tick on release, so 500 ms already elapsed
     *-50ms as lv_test_mouse_click_at has 50ms delay after release.
     *Use the same image as we are just waiting for the delay*/
    lv_test_wait(1000 + 300 - 500 - 50);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_5.png");
    lv_test_wait(300);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_6.png");
    lv_test_wait(200);

    /*Animated back to normal size, but checked*/
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_7.png");

    /*Click Button 1 to trigger a load animation,
     *but Button 2 is now checked and loaded from the bottom*/
    lv_test_mouse_click_at(45, 45);
    lv_test_wait(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_8.png");
    lv_test_wait(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_9.png");
    lv_test_wait(500);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/timeline_10.png");
}

#endif
