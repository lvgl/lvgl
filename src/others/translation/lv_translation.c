/**
 * @file lv_translation.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_translation.h"
#if LV_USE_TRANSLATION

#include "lv_translation_private.h"
#include "../../misc/lv_ll.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_assert.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define packs_ll (LV_GLOBAL_DEFAULT()->translation_packs_ll)
#define selected_lang (LV_GLOBAL_DEFAULT()->translation_selected_lang)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_translation_init(void)
{
    lv_ll_init(&packs_ll, sizeof(lv_translation_pack_t));
    selected_lang = NULL;
}

void lv_translation_deinit(void)
{
    lv_translation_pack_t * pack;
    LV_LL_READ(&packs_ll, pack) {
        if(pack->is_static == false) {
            size_t i;
            size_t trans_cnt = lv_array_size(&pack->translation_array);
            for(i = 0; i < trans_cnt; i++) {
                lv_translation_tag_dsc_t * tag = lv_array_at(&pack->translation_array, i);
                lv_free((void *)tag->tag);

                size_t j;
                for(j = 0; j < pack->language_cnt; j++) {
                    lv_free((void *)tag->translations[j]); /*Free each translation of the tag*/
                }
                lv_free(tag->translations);
            }

            lv_array_deinit(&pack->translation_array);

            for(i = 0; i < pack->language_cnt; i++) {
                lv_free((void *)pack->languages[i]);
            }
            lv_free(pack->languages);
        }
    }

    lv_ll_clear(&packs_ll);

    lv_free((void *)selected_lang);
}

lv_translation_pack_t * lv_translation_add_static(const char * languages[], const char * tags[],
                                                  const char * translations[])
{
    LV_ASSERT_NULL(languages);
    LV_ASSERT_NULL(tags);
    LV_ASSERT_NULL(translations);

    lv_translation_pack_t * pack = lv_ll_ins_head(&packs_ll);
    LV_ASSERT_MALLOC(pack);
    if(pack == NULL) return NULL;
    lv_memzero(pack, sizeof(lv_translation_pack_t));
    pack->is_static = 1;

    /*Count the languages*/
    while(languages[pack->language_cnt]) {
        pack->language_cnt++;
    }

    pack->languages = languages;
    pack->tag_p = tags;
    pack->translation_p = translations;
    return pack;
}

lv_translation_pack_t * lv_translation_add_dynamic(void)
{
    lv_translation_pack_t * pack = lv_ll_ins_head(&packs_ll);
    LV_ASSERT_MALLOC(pack);
    if(pack == NULL) return NULL;

    lv_memzero(pack, sizeof(lv_translation_pack_t));

    pack->is_static = 0;
    lv_array_init(&pack->translation_array, 16, sizeof(lv_translation_tag_dsc_t));

    return pack;
}

void lv_translation_set_language(const char * lang)
{
    if(selected_lang) lv_free((void *)selected_lang);
    selected_lang = lv_strdup(lang);
}

const char * lv_translation_get(const char * tag)
{
    if(selected_lang == NULL) {
        LV_LOG_WARN("No language is selected to get the translation of `%s`", tag);
        return tag;
    }

    lv_translation_pack_t * pack;
    bool lang_found = false;
    LV_LL_READ(&packs_ll, pack) {
        uint32_t lang;
        for(lang = 0; lang < pack->language_cnt; lang++) {
            /*Does this pack contains the language?*/
            if(lv_streq(pack->languages[lang], selected_lang)) {
                lang_found = true;
                /*Find the tag*/
                if(pack->is_static) {
                    uint32_t t;
                    for(t = 0; pack->tag_p[t]; t++) {
                        if(lv_streq(pack->tag_p[t], tag)) {
                            /*Find the "row" of the tag */
                            const char ** tr_row = pack->translation_p + pack->language_cnt * t;
                            const char * tr = tr_row[lang];
                            if(tr) return tr; /*Found directly*/

                            LV_LOG_WARN("`%s` tag is not found. Using the tag as translation.", tag);
                            return tag; /*Return the tag as a fall back*/
                        }
                    }
                }
                else {
                    size_t trans_cnt = lv_array_size(&pack->translation_array);
                    size_t i;
                    for(i = 0; i < trans_cnt; i++) {
                        lv_translation_tag_dsc_t * tag_dsc = lv_array_at(&pack->translation_array, i);
                        if(lv_streq(tag_dsc->tag, tag)) {
                            const char * tr = tag_dsc->translations[lang];
                            if(tr) return tr; /*Found directly*/

                            LV_LOG_WARN("`%s` tag is not found. Using the tag as translation.", tag);
                            return tag; /*Return the tag as a worst case option*/
                        }
                    }
                }
            }
        }
    }

    if(lang_found) {
        LV_LOG_WARN("`%s` tag is not found, using the tag as translation.", tag);
    }
    else {
        LV_LOG_WARN("`%s` language is not found, using the `%s` as translation.", selected_lang, tag);
    }

    return tag;
}

lv_result_t lv_translation_add_language(lv_translation_pack_t * pack, const char * lang)
{
    if(pack->is_static) {
        LV_LOG_WARN("Can't add language `%s` to static translation pack `%p`", lang, (void *)pack);
        return LV_RESULT_INVALID;
    }

    pack->language_cnt++;
    pack->languages = lv_realloc(pack->languages, sizeof(const char *) * pack->language_cnt);
    LV_ASSERT_MALLOC(pack->languages);
    if(pack->languages == NULL) {
        LV_LOG_WARN("Couldn't allocate languages in `%p`", (void *)pack);
        return LV_RESULT_INVALID;
    }

    pack->languages[pack->language_cnt - 1] = lv_strdup(lang);
    LV_ASSERT_MALLOC(pack->languages[pack->language_cnt - 1]);
    if(pack->languages[pack->language_cnt - 1] == NULL) {
        LV_LOG_WARN("Couldn't allocate the new language in `%p`", (void *)pack);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

int32_t lv_translation_get_language_index(lv_translation_pack_t * pack, const char * lang_name)
{
    uint32_t i;
    for(i = 0; i < pack->language_cnt; i++) {
        if(lv_streq(pack->languages[i], lang_name)) return (int32_t)i;
    }

    return -1;
}


lv_translation_tag_dsc_t * lv_translation_add_tag(lv_translation_pack_t * pack, const char * tag_name)
{
    if(pack->is_static) {
        LV_LOG_WARN("Can't add tag `%s` to static translation pack `%p`", tag_name, (void *)pack);
        return NULL;
    }

    lv_translation_tag_dsc_t tag;
    tag.tag = lv_strdup(tag_name);
    LV_ASSERT_MALLOC(tag.tag);
    tag.translations = lv_zalloc(pack->language_cnt * sizeof(const char *));
    LV_ASSERT_MALLOC(tag.translations);

    if(tag.tag == NULL || tag.translations == NULL) {
        LV_LOG_WARN("Couldn't allocate memory for the tag's data in `%p`", (void *)pack);
        lv_free((void *)tag.tag);
        lv_free((void *)tag.translations);
        return NULL;
    }

    lv_result_t res = lv_array_push_back(&pack->translation_array, &tag);

    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Couldn't add the tag in `%p`", (void *)pack);
        lv_free((void *)tag.tag);
        lv_free((void *)tag.translations);
        return NULL;
    }

    return lv_array_back(&pack->translation_array);
}

lv_result_t lv_translation_set_tag_translation(lv_translation_pack_t * pack, lv_translation_tag_dsc_t * tag,
                                               uint32_t lang_idx, const char * trans)
{
    if(pack->is_static) {
        LV_LOG_WARN("Can't set tag translation`%s` in static translation pack `%p`", trans, (void *)pack);
        return LV_RESULT_INVALID;
    }

    if(lang_idx >= pack->language_cnt) {

        LV_LOG_WARN("Can't set the translation for language %" LV_PRIu32 " as there are only %" LV_PRIu32
                    " languages defined in %p",
                    lang_idx, pack->language_cnt, (void *)pack);
        return LV_RESULT_INVALID;
    }

    lv_free((void *)tag->translations[lang_idx]); /*Free the earlier set language if any*/
    tag->translations[lang_idx] = lv_strdup(trans);
    if(tag->translations[lang_idx] == NULL) {
        LV_LOG_WARN("Couldn't allocate the new translation in tag `%p` in pack `%p`", (void *)tag, (void *) pack);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TRANSLATION*/
