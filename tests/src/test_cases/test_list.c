#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * list;

void setUp(void)
{
    list = lv_list_create(lv_scr_act());
}

void tearDown(void)
{

}

void test_list_get_text_from_added_button(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * button_ok = lv_list_add_btn(list, LV_SYMBOL_OK, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_list_get_btn_text(list, button_ok));
}

void test_list_get_text_from_button_without_symbol(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * button_ok = lv_list_add_btn(list, NULL, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_list_get_btn_text(list, button_ok));
}

void test_list_gets_empty_text_from_button_without_text(void)
{
    const char * empty_text = "";
    lv_obj_t * button_ok = lv_list_add_btn(list, NULL, NULL);

    TEST_ASSERT_EQUAL_STRING(empty_text, lv_list_get_btn_text(list, button_ok));
}

void test_list_get_text_from_label(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * label = lv_list_add_text(list, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_label_get_text(label));
}

#endif
