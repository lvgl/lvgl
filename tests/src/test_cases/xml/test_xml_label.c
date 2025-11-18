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

void test_xml_label_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * label1_attrs[] = {
        "text", "This is the text with ellipses added automatically",
        "long_mode", "dots",
        "width", "100",
        "height", "40",
        "align", "center",
        "style_bg_opa", "50%",
        "style_bg_color", "0xf00",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", label1_attrs);

    static lv_subject_t s1;
    lv_subject_init_int(&s1, 20);
    lv_xml_register_subject(NULL, "s1", &s1);


    const char * label2_attrs[] = {
        "bind_text", "s1",
        "bind_text-fmt", "We have %d users",
        "y", "10",
        "x", "5",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", label2_attrs);

    static lv_subject_t s2;
    lv_subject_init_float(&s2, 12.3f);
    lv_xml_register_subject(NULL, "s2", &s2);

    const char * label3_attrs[] = {
        "bind_text", "s2",
        "bind_text-fmt", "We have measured: %0.3f mW",
        "y", "30",
        "x", "5",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_label", label3_attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_label.png");
}
void test_xml_label_translation_tag(void)
{
    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = { "The Tiger", "Der Tiger", "El Tigre" };
    lv_translation_add_static(languages, tags, translations);

    lv_obj_t * scr = lv_screen_active();

    const char * label1_attrs[] = {
        "text", "",
        "translation_tag", "tiger",
        NULL, NULL,
    };

    const char * label2_attrs[] = {
        "text", "This is text",
        "translation_tag", "",
        NULL, NULL,
    };

    const char * label3_attrs[] = {
        "text", "This is text",
        "translation_tag", "tiger",
        NULL, NULL,
    };

    const char * label4_attrs[] = {
        "translation_tag", "tiger",
        "text", "This is text",
        NULL, NULL,
    };

    const char * label5_attrs[] = {
        "translation_tag", "",
        "text", "",
        NULL, NULL,
    };

    lv_obj_t * label = lv_xml_create(scr, "lv_label", label1_attrs);
    lv_obj_t * label2 = lv_xml_create(scr, "lv_label", label2_attrs);
    lv_obj_t * label3 = lv_xml_create(scr, "lv_label", label3_attrs);
    lv_obj_t * label4 = lv_xml_create(scr, "lv_label", label4_attrs);
    lv_obj_t * label5 = lv_xml_create(scr, "lv_label", label5_attrs);
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label2), "This is text");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label3), "Der Tiger");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label4), "This is text");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label5), "");
}

void test_xml_label_both_text_and_translation_tag(void)
{
    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = { "The Tiger", "Der Tiger", "El Tigre" };
    lv_translation_add_static(languages, tags, translations);

    lv_obj_t * scr = lv_screen_active();
    const char * label1_attrs[] = {
        "translation_tag", "tiger",
        NULL, NULL,
    };
    lv_obj_t * label = lv_xml_create(scr, "lv_label", label1_attrs);
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");
}

#endif
