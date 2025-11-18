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

    lv_xml_register_translation_from_data(translations_xml);


    static const char * languages[] = {"en", "de", "es", NULL};
    static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", "Der Löwe", "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant", "El Elefante",
    };

    lv_translation_pack_t * pack = lv_translation_add_static(languages, tags, translations);

    TEST_ASSERT_EQUAL_INT(1, lv_translation_get_language_index(pack, "de"));
    TEST_ASSERT_EQUAL_INT(-1, lv_translation_get_language_index(pack, "none"));

    lv_translation_set_language("de");

    TEST_ASSERT_EQUAL_STRING("Der Hund", lv_translation_get("dog"));
    TEST_ASSERT_EQUAL_STRING("Der Löwe", lv_translation_get("lion"));

    /*The tag the fallback if not defined for the selected language*/
    TEST_ASSERT_EQUAL_STRING("cat", lv_translation_get("cat"));

    /*Use the tag if the tag is not found*/
    TEST_ASSERT_EQUAL_STRING("foo", lv_tr("foo"));

    lv_translation_set_language("es");

    TEST_ASSERT_EQUAL_STRING("El Conejo", lv_tr("rabbit"));
}

#endif
