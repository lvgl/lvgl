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

void test_xml_translation(void)
{
    const char * translations_xml = {
        "<translations languages=\"en de hu\">"
        "  <translation tag=\"dog\"       en=\"The dog\"    de=\"Der Hund\"     hu=\"A kutya\"/>"
        "  <translation tag=\"cat\"       en=\"The cat\"                        hu=\"A cica\"/>"
        "  <translation tag=\"snake\"     en=\"A snake\"    de=\"Eine Schlange\" hu=\"A kígyó\"/>"
        "</translations>"
    };

    lv_xml_translation_register_from_data(translations_xml);


    static const char * languages[] = {"en", "de", "es", NULL};
    static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", "Der Löwe", "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant", "El Elefante",
    };

    lv_xml_translation_register_from_array(languages, tags, translations);


    lv_xml_set_language("de");

    TEST_ASSERT_EQUAL_STRING("Der Hund", lv_xml_get_translation("dog"));
    TEST_ASSERT_EQUAL_STRING("Der Löwe", lv_xml_get_translation("lion"));

    /*The 1st language is the fallback*/
    TEST_ASSERT_EQUAL_STRING("The cat", lv_xml_get_translation("cat"));

    /*Use the tag if not found*/
    TEST_ASSERT_EQUAL_STRING("foo", lv_xml_get_translation("foo"));


    lv_xml_set_language("es");

    TEST_ASSERT_EQUAL_STRING("El Conejo", lv_xml_get_translation("rabbit"));

}

#endif
