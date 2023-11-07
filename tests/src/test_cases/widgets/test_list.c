#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * list;

void setUp(void)
{
    list = lv_list_create(lv_screen_active());
}

void tearDown(void)
{

}

void test_list_get_text_from_added_button(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * button_ok = lv_list_add_button(list, LV_SYMBOL_OK, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_list_get_button_text(list, button_ok));
}

void test_list_get_text_from_button_without_symbol(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * button_ok = lv_list_add_button(list, NULL, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_list_get_button_text(list, button_ok));
}

void test_list_gets_empty_text_from_button_without_text(void)
{
    const char * empty_text = "";
    lv_obj_t * button_ok = lv_list_add_button(list, NULL, NULL);

    TEST_ASSERT_EQUAL_STRING(empty_text, lv_list_get_button_text(list, button_ok));
}

void test_list_get_text_from_label(void)
{
    const char * message = "LVGL Rocks!";
    lv_obj_t * label = lv_list_add_text(list, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_label_get_text(label));
}

void test_list_snapshot(void)
{
    lv_obj_t * snapshot_list = lv_list_create(lv_screen_active());

    lv_list_add_text(snapshot_list, "File");
    lv_list_add_button(snapshot_list, LV_SYMBOL_FILE, "New");
    lv_list_add_button(snapshot_list, LV_SYMBOL_DIRECTORY, "Open");
    lv_list_add_button(snapshot_list, LV_SYMBOL_SAVE, "Save");

    lv_obj_center(snapshot_list);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/list_1.png");
}

#endif
