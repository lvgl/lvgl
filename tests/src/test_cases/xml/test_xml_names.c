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

void test_xml_names(void)
{

    const char * base_label_xml =
        "<component>"
        "  <view extends=\"lv_label\"></view>"
        "</component>";

    const char * my_label_xml =
        "<component>"
        "  <view extends=\"base_label\"></view>"
        "</component>";

    const char * my_btn_xml =
        "<component>"
        "  <view extends=\"lv_button\">"
        "    <my_label name=\"first_label\"/>"
        "    <my_label/>"
        "    <my_label name=\"third_label\"/>"
        "    <my_label/>"
        "  </view>"
        "</component>";

    const char * main_screen_xml =
        "<screen>"
        "  <view name=\"main_screen\">"
        "    <my_btn/>"
        "    <my_btn name=\"first_btn\"/>"
        "    <lv_button/>"
        "    <my_btn/>"
        "    <my_btn name=\"lv_button_#\"/>"
        "    <lv_button/>"
        "  </view>"
        "</screen>";

    lv_xml_component_register_from_data("my_btn", my_btn_xml);
    lv_xml_component_register_from_data("base_label", base_label_xml);
    lv_xml_component_register_from_data("my_label", my_label_xml);
    lv_xml_component_register_from_data("main_screen", main_screen_xml);

    lv_obj_t * main_screen = lv_xml_create(NULL, "main_screen", NULL);


    char buf[128];
    lv_obj_t * btn;
    btn = lv_obj_get_child(main_screen, 0);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("my_btn_0", buf);

    btn = lv_obj_get_child(main_screen, 1);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("first_btn", buf);

    btn = lv_obj_get_child(main_screen, 2);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("lv_button_0", buf);

    btn = lv_obj_get_child(main_screen, 3);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("my_btn_1", buf);

    btn = lv_obj_get_child(main_screen, 4);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("lv_button_1", buf);

    btn = lv_obj_get_child(main_screen, 5);
    lv_obj_get_name_resolved(btn, buf, 128);
    TEST_ASSERT_EQUAL_STRING("lv_button_2", buf);


    btn = lv_obj_get_child(main_screen, 3);
    lv_obj_t * label;
    label = lv_obj_get_child(btn, 0);
    lv_obj_get_name_resolved(label, buf, 128);
    TEST_ASSERT_EQUAL_STRING("first_label", buf);

    label = lv_obj_get_child(btn, 1);
    lv_obj_get_name_resolved(label, buf, 128);
    TEST_ASSERT_EQUAL_STRING("my_label_0", buf);

    label = lv_obj_get_child(btn, 2);
    lv_obj_get_name_resolved(label, buf, 128);
    TEST_ASSERT_EQUAL_STRING("third_label", buf);

    label = lv_obj_get_child(btn, 3);
    lv_obj_get_name_resolved(label, buf, 128);
    TEST_ASSERT_EQUAL_STRING("my_label_1", buf);

}

#endif
