#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    lv_translation_deinit();
    lv_translation_init();
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/**********************
 *   HELPERS
 **********************/

static const char * const languages[] = {"en", "de", "es", NULL};
static const char * const tags[] = {"tiger", "lion", NULL};
static const char * const translations[] = {
    "The Tiger", "Der Tiger", "El Tigre",
    "The Lion",  "Der Löwe",  "El León",
};

static lv_translation_pack_t * add_animals_pack(void)
{
    return lv_translation_add_static(languages, tags, translations);
}

/* Build the equivalent of `add_animals_pack` with the dynamic API. */
static lv_translation_pack_t * add_animals_pack_dynamic(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    TEST_ASSERT_NOT_NULL(pack);

    uint32_t lang_cnt = 0;
    while(languages[lang_cnt]) {
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, languages[lang_cnt]));
        lang_cnt++;
    }

    uint32_t t;
    for(t = 0; tags[t]; t++) {
        lv_translation_tag_dsc_t * tag = lv_translation_add_tag(pack, tags[t]);
        TEST_ASSERT_NOT_NULL(tag);

        uint32_t lang;
        for(lang = 0; lang < lang_cnt; lang++) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK,
                              lv_translation_set_tag_translation(pack, tag, lang, translations[t * lang_cnt + lang]));
        }
    }

    return pack;
}

static void assert_animals_translate(void)
{
    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("The Tiger", lv_tr("tiger"));
    TEST_ASSERT_EQUAL_STRING("The Lion", lv_tr("lion"));

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("Der Tiger", lv_tr("tiger"));
    TEST_ASSERT_EQUAL_STRING("Der Löwe", lv_tr("lion"));

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING("El Tigre", lv_tr("tiger"));
    TEST_ASSERT_EQUAL_STRING("El León", lv_tr("lion"));
}

/**********************
 *   LANGUAGE SELECTION
 **********************/

void test_no_language_is_selected_initially(void)
{
    TEST_ASSERT_NULL(lv_translation_get_language());
}

void test_set_language_is_reported_back(void)
{
    add_animals_pack();

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("de", lv_translation_get_language());

    /* An unknown language is still selected, it just won't resolve any tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING("fr", lv_translation_get_language());
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
    add_animals_pack();

    lv_obj_t * label = lv_label_create(NULL);
    lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, "tiger");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("The Tiger", lv_label_get_text(label));

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("Der Tiger", lv_label_get_text(label));

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING("El Tigre", lv_label_get_text(label));

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING("tiger", lv_label_get_text(label));

    lv_obj_delete(label);
}

/**********************
 *   STATIC PACKS
 **********************/

void test_static_pack_translates_all_languages(void)
{
    TEST_ASSERT_NOT_NULL(add_animals_pack());
    assert_animals_translate();
}

void test_get_without_selected_language_returns_the_tag(void)
{
    add_animals_pack();
    TEST_ASSERT_EQUAL_STRING("tiger", lv_tr("tiger"));
}

void test_get_unknown_tag_returns_the_tag(void)
{
    add_animals_pack();

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("zebra", lv_tr("zebra"));
}

void test_get_with_unknown_language_returns_the_tag(void)
{
    add_animals_pack();

    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING("tiger", lv_tr("tiger"));
}

void test_missing_translation_returns_the_tag(void)
{
    static const char * const sparse_langs[] = {"en", "de", NULL};
    static const char * const sparse_tags[] = {"cat", NULL};
    /* No German translation for "cat" */
    static const char * const sparse_translations[] = {"The Cat", NULL};

    lv_translation_add_static(sparse_langs, sparse_tags, sparse_translations);

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("The Cat", lv_tr("cat"));

    /* The tag is used as a fallback, the first language is *not* consulted */
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("cat", lv_tr("cat"));
}

void test_static_pack_rejects_add_language(void)
{
    lv_translation_pack_t * pack = add_animals_pack();
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_translation_add_language(pack, "fr"));
}

void test_static_pack_rejects_add_tag(void)
{
    lv_translation_pack_t * pack = add_animals_pack();
    TEST_ASSERT_NULL(lv_translation_add_tag(pack, "zebra"));
}

void test_static_pack_rejects_set_tag_translation(void)
{
    lv_translation_pack_t * static_pack = add_animals_pack();

    /* A valid tag descriptor is needed, borrow one from a dynamic pack */
    lv_translation_pack_t * dynamic_pack = lv_translation_add_dynamic();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(dynamic_pack, "en"));
    lv_translation_tag_dsc_t * tag = lv_translation_add_tag(dynamic_pack, "cat");
    TEST_ASSERT_NOT_NULL(tag);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_translation_set_tag_translation(static_pack, tag, 0, "The Cat"));
}


/**********************
 *   DYNAMIC PACKS
 **********************/

void test_dynamic_pack_translates_all_languages(void)
{
    add_animals_pack_dynamic();
    assert_animals_translate();
}

void test_dynamic_pack_missing_translation_returns_the_tag(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, "en"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, "de"));

    lv_translation_tag_dsc_t * tag = lv_translation_add_tag(pack, "cat");
    TEST_ASSERT_NOT_NULL(tag);
    /* Only "en" is set */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_set_tag_translation(pack, tag, 0, "The Cat"));

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("The Cat", lv_tr("cat"));

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("cat", lv_tr("cat"));
}

void test_set_tag_translation_overwrites_the_previous_one(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, "en"));

    lv_translation_tag_dsc_t * tag = lv_translation_add_tag(pack, "cat");
    TEST_ASSERT_NOT_NULL(tag);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_set_tag_translation(pack, tag, 0, "The Cat"));
    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("The Cat", lv_tr("cat"));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_set_tag_translation(pack, tag, 0, "The Kitten"));
    TEST_ASSERT_EQUAL_STRING("The Kitten", lv_tr("cat"));
}

void test_set_tag_translation_rejects_out_of_range_language(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, "en"));

    lv_translation_tag_dsc_t * tag = lv_translation_add_tag(pack, "cat");
    TEST_ASSERT_NOT_NULL(tag);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_translation_set_tag_translation(pack, tag, 1, "Die Katze"));
}

void test_deinit_releases_dynamic_packs(void)
{
    add_animals_pack_dynamic();

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("Der Tiger", lv_tr("tiger"));

    lv_translation_deinit();
    lv_translation_init();

    /* No packs and no selected language left */
    TEST_ASSERT_NULL(lv_translation_get_language());
    TEST_ASSERT_EQUAL_STRING("tiger", lv_tr("tiger"));
}

/**********************
 *   LANGUAGE INDEX
 **********************/

void test_get_language_index_of_static_pack(void)
{
    lv_translation_pack_t * pack = add_animals_pack();

    TEST_ASSERT_EQUAL_INT32(0, lv_translation_get_language_index(pack, "en"));
    TEST_ASSERT_EQUAL_INT32(1, lv_translation_get_language_index(pack, "de"));
    TEST_ASSERT_EQUAL_INT32(2, lv_translation_get_language_index(pack, "es"));
    TEST_ASSERT_EQUAL_INT32(-1, lv_translation_get_language_index(pack, "fr"));
}

void test_get_language_index_of_dynamic_pack(void)
{
    lv_translation_pack_t * pack = add_animals_pack_dynamic();

    TEST_ASSERT_EQUAL_INT32(0, lv_translation_get_language_index(pack, "en"));
    TEST_ASSERT_EQUAL_INT32(1, lv_translation_get_language_index(pack, "de"));
    TEST_ASSERT_EQUAL_INT32(2, lv_translation_get_language_index(pack, "es"));
    TEST_ASSERT_EQUAL_INT32(-1, lv_translation_get_language_index(pack, "fr"));
}

/**********************
 *   MULTIPLE PACKS
 **********************/

void test_tags_are_resolved_across_packs(void)
{
    static const char * const other_langs[] = {"en", "de", NULL};
    static const char * const other_tags[] = {"cat", NULL};
    static const char * const other_translations[] = {"The Cat", "Die Katze"};

    add_animals_pack();
    lv_translation_add_static(other_langs, other_tags, other_translations);

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("Der Tiger", lv_tr("tiger"));
    TEST_ASSERT_EQUAL_STRING("Die Katze", lv_tr("cat"));
}

void test_last_added_pack_wins_for_a_duplicated_tag(void)
{
    static const char * const override_langs[] = {"en", NULL};
    static const char * const override_tags[] = {"tiger", NULL};
    static const char * const override_translations[] = {"A Different Tiger"};

    add_animals_pack();
    lv_translation_add_static(override_langs, override_tags, override_translations);

    /* Packs are prepended, so the most recently added one shadows the earlier ones */
    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING("A Different Tiger", lv_tr("tiger"));

    /* The shadowing pack has no "de", so the earlier pack still resolves it */
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING("Der Tiger", lv_tr("tiger"));
}

#endif
