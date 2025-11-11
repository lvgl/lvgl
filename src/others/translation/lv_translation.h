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
lv_translation_pack_t * lv_translation_add_static(const char * const languages[], const char * const tags[],
                                                  const char * const translations[]);

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
 * The `LV_EVENT_TRANSLATION_LANGUAGE_CHANGED` event will be sent to every widget
 * @param lang      a string from the defined languages. E.g. "en" or "de"
 */
void lv_translation_set_language(const char * lang);

/**
 * Get the current selected language
 * @return the current selected language
 */
const char * lv_translation_get_language(void);

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

/**
 * Add a new language to a dynamic language pack.
 * All languages should be added before adding tags
 * @param pack      pointer to a dynamic translation pack
 * @param lang      language to add, e.g. "en", or "de"
 * @return          LV_RESULT_OK: success, LV_RESULT_INVALID: failed
 */
lv_result_t lv_translation_add_language(lv_translation_pack_t * pack, const char * lang);

/**
 * Get the index of a language in a pack.
 * @param pack      pointer to a static or dynamic language pack
 * @param lang_name name of the language to find
 * @return          index of the language or -1 if not found.
 */
int32_t lv_translation_get_language_index(lv_translation_pack_t * pack, const char * lang_name);

/**
 * Add a new tag to a dynamic language pack.
 * Once the tag is added the translations for each language can be added too by using
 * `lv_translation_set_tag_translation`
 * @param pack      pointer to a dynamic translation pack
 * @param tag_name  name of the tag, e.g. "dog", or "house"
 * @return          pointer to the allocated tag descriptor
 */
lv_translation_tag_dsc_t * lv_translation_add_tag(lv_translation_pack_t * pack, const char * tag_name);

/**
 * Add a translation to a tag in a dynamic translation pack
 * @param pack      pointer to a dynamic translation pack
 * @param tag       return value of `lv_translation_add_tag`
 * @param lang_idx  index of the language for which translation should be set
 * @param trans     the translation on the given language
 * @return          LV_RESULT_OK: success, LV_RESULT_INVALID: failed
 */
lv_result_t lv_translation_set_tag_translation(lv_translation_pack_t * pack, lv_translation_tag_dsc_t * tag,
                                               uint32_t lang_idx, const char * trans);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_TRANSLATION*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_TRANSLATION_H */
