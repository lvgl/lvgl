#ifndef LV_I18N_H
#define LV_I18N_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lvgl.h"
#if LV_USE_DEMO_EBIKE

#include LV_STDINT_INCLUDE

typedef enum {
    LV_I18N_PLURAL_TYPE_ZERO,
    LV_I18N_PLURAL_TYPE_ONE,
    LV_I18N_PLURAL_TYPE_TWO,
    LV_I18N_PLURAL_TYPE_FEW,
    LV_I18N_PLURAL_TYPE_MANY,
    LV_I18N_PLURAL_TYPE_OTHER,
    _LV_I18N_PLURAL_TYPE_NUM,
} lv_i18n_plural_type_t;

typedef struct {
    const char * msg_id;
    const char * translation;
} lv_i18n_phrase_t;

typedef struct {
    const char * locale_name;
    lv_i18n_phrase_t * singulars;
    lv_i18n_phrase_t * plurals[_LV_I18N_PLURAL_TYPE_NUM];
    uint8_t (*locale_plural_fn)(int32_t num);
} lv_i18n_lang_t;

// Null-terminated list of languages. First one used as default.
typedef const lv_i18n_lang_t * lv_i18n_language_pack_t;


extern const lv_i18n_language_pack_t lv_i18n_language_pack[];


/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs);

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en_GB"
 */
int lv_i18n_set_locale(const char * l_name);

/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id);

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num);

/**
 * Get the name of the currently used localization.
 * @return name of the currently used localization. E.g. "en_GB"
 */
const char * lv_i18n_get_current_locale(void);


void __lv_i18n_reset(void);


#define _(text) lv_i18n_get_text(text)
#define _p(text, num) lv_i18n_get_text_plural(text, num)

#endif /*LV_USE_DEMO_EBIKE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LANG_H*/
