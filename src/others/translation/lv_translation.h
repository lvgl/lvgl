/**
 * @file lv_translation.h
 *
 */

#ifndef LV_TRANSLATION_H
#define LV_TRANSLATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_TRANSLATION

#include LV_STDINT_INCLUDE
#include "../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * tag;
    const char ** translations; /**< Translations for each language*/
} lv_translation_tag_dsc_t;

typedef struct {
    const char ** languages;
    uint32_t language_cnt;
    uint32_t is_static; /*In the union translations_p is used*/
    const char ** tag_p;
    const char ** translation_p; /*E.g. {{"a", "b"}, {"c", "d"}}*/
    lv_array_t translation_array;
} lv_translation_pack_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the translation module
 */
void lv_translation_init(void);

/**
 * De-initialize the translation module and free all allocated translations
 */
void lv_translation_deinit(void);

/**
 * Register a translation pack from static arrays.
 * All the pointers need to be static, that is to live while they are used
 * @param languages     List of languages. E.g. `{"en", "de", NULL}`
 * @param tags          Tags that are using in the UI. E.g. `{"dog", "cat", NULL}`
 * @param translations  List of translations. E.g. `{"Dog", "Cat", "Hund", "Katze"}`
 * @return              The created pack
 */
lv_translation_pack_t * lv_translation_add_static(const char * languages[], const char * tags[],
                                                  const char * translations[]);

/**
 * Add a pack to which translations can be added dynamically.
 * `pack->languages` needs to be a malloc-ed array where each language is also malloc-ed as an element.
 * `pack->translation_array` stores the translation having `lv_translation_tag_dsc_t` items
 * In each array element `tag` is a malloced string, `translations` is a malloc-ed array
 * with malloc-ed array for each element.
 * @return      the created pack to which data can be added manually.
 */
lv_translation_pack_t * lv_translation_add_dynamic(void);

/**
 * Select the current language
 * @param lang      a string from the defined languages. E.g. "en" or "de"
 */
void lv_translation_set_language(const char * lang);

/**
 * Get the translated version of a tag on the selected language
 * @param tag       the tag to translate
 * @return          the translation
 * @note            fallback rules:
 *                      - if the tag is found on the selected language return it
 *                      - if the tag is not found on the selected language, use the fist language
 *                      - if the tag is not found on the first language, return the tag
 */
const char * lv_translation_get(const char * tag);

/**
 * Shorthand of lv_translation_set_language
 * @param tag       the tag to translate
 * @return          the translation
 */
static inline const char * lv_tr(const char * tag)
{
    return lv_translation_get(tag);
}

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_TRANSLATION*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_TRANSLATION_H */
