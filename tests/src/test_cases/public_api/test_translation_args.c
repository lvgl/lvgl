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

static const char * const languages[] = {"en", "de", "es", NULL};
static const char * const tags[] = {"tiger", "lion", NULL};
static const char * const translations[] = {
    "The Tiger", "Der Tiger", "El Tigre",
    "The Lion",  "Der Löwe",  "El León",
};

/* A static pack rejects every mutation, it is built from constant data */
static lv_translation_pack_t * add_animals_pack(void)
{
    return lv_translation_add_static(languages, tags, translations);
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

void test_set_tag_translation_rejects_out_of_range_language(void)
{
    lv_translation_pack_t * pack = lv_translation_add_dynamic();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_translation_add_language(pack, "en"));

    lv_translation_tag_dsc_t * tag = lv_translation_add_tag(pack, "cat");
    TEST_ASSERT_NOT_NULL(tag);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_translation_set_tag_translation(pack, tag, 1, "Die Katze"));
}

#endif /*LV_BUILD_TEST*/
