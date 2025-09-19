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

static void on_language_change(lv_event_t * e)
{
    lv_obj_t * label      = lv_event_get_target_obj(e);
    const char * tag      = lv_event_get_user_data(e);
    const char * language = lv_event_get_param(e);

    lv_label_set_text(label, lv_tr(tag));
    TEST_ASSERT_EQUAL_STRING(language, lv_translation_get_language());
}

void test_set_language_sends_language_changed_event(void)
{

    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = { "The Tiger", "Der Tiger", "El Tigre" };
    lv_translation_add_static(languages, tags, translations);

    lv_obj_t * label = lv_label_create(NULL);
    lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, "tiger");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "The Tiger");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "El Tigre");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "tiger");
}

#endif
