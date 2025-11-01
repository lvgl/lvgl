#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

void test_list_translation_tag(void)
{
    static const char * languages[] = {"en", "de", "es", NULL};
    static const char * tags[] = {"tiger", "lion", "rabbit", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", "Der Löwe", "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo"
    };
    lv_translation_add_static(languages, tags, translations);

    lv_obj_t * list_text = lv_list_add_translation_tag(list, "tiger");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(list_text), "The Tiger");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(list_text), "Der Tiger");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(list_text), "El Tigre");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(list_text), "tiger");

    lv_obj_t * list_button = lv_list_add_button_translation_tag(list, NULL, "lion");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "The Lion");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "Der Löwe");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "El León");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "lion");

    lv_list_set_button_translation_tag(list, list_button, "rabbit");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "The Rabbit");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "Das Kaninchen");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "El Conejo");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_list_get_button_text(list, list_button), "rabbit");

}

#endif
