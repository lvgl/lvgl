#include "../../lv_examples.h"
#if LV_USE_TRANSLATION && LV_BUILD_EXAMPLES

static void add_static(void)
{
    static const char * languages[] = {"en", "de", "es", NULL};
    static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", "Der Löwe", "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant", "El Elefante",
    };

    lv_translation_add_static(languages, tags, translations);
}

static void add_dynamic(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    pack->language_cnt = 2;
    pack->languages = lv_malloc(pack->language_cnt * sizeof(const char *));
    pack->languages[0] = lv_strdup("en");
    pack->languages[1] = lv_strdup("de");

    lv_translation_tag_dsc_t tag1;
    tag1.tag = lv_strdup("table");
    tag1.translations = lv_malloc(2 * sizeof(const char *) * pack->language_cnt);
    tag1.translations[0] = lv_strdup("It's a table");
    tag1.translations[1] = lv_strdup("Das is ein Tish");
    lv_array_push_back(&pack->translation_array, &tag1);

    lv_translation_tag_dsc_t tag2;
    tag2.tag = lv_strdup("chair");
    tag2.translations = lv_malloc(2 * sizeof(const char *) * pack->language_cnt);
    tag2.translations[0] = lv_strdup("It's a chair");
    tag2.translations[1] = lv_strdup("Das ist ein Stuhl");
    lv_array_push_back(&pack->translation_array, &tag2);
}

/**
 * Create and use translations
 */
void lv_example_translation_1(void)
{
    add_static();
    add_dynamic();

    lv_translation_set_language("de");

    lv_obj_t * label;

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, lv_tr("tiger"));

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, lv_tr("chair"));
    lv_obj_set_y(label, 50);
}

#endif /*LV_USE_TRANSLATION && LV_BUILD_EXAMPLES*/
