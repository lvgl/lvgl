/**
 * @file lv_lang.h
 *
 */

#ifndef LV_I18N_H
#define LV_I18N_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_I18N

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_I18N_PLURAL_TYPE_ZERO,
    LV_I18N_PLURAL_TYPE_ONE,
    LV_I18N_PLURAL_TYPE_TWO,
    LV_I18N_PLURAL_TYPE_FEW,
    LV_I18N_PLURAL_TYPE_MANY,
    LV_I18N_PLURAL_TYPE_OTHER,
    _LV_I18N_PLURAL_TYPE_NUM,
}lv_i18n_plural_type_t;

typedef struct {
    const char * msg_id;
    const char * txt_trans;
}lv_i18n_trans_t;


typedef struct {
    const char * name;                                      /*E.g. "en_GB"*/
    lv_i18n_trans_t * simple;                               /*Translations of simple texts where no plurals are used*/
    lv_i18n_trans_t * plurals[_LV_I18N_PLURAL_TYPE_NUM];    /*Translations of the plural forms*/
    uint8_t (*plural_rule)(int32_t num);                    /*Function pointer to get the correct plural form for a number*/
}lv_i18n_lang_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
void lv_i18n_init(const lv_i18n_lang_t ** langs);

/**
 * Change the localization (language)
 * @param lang_code name of the translation to use. E.g. "en_GB"
 */
void lv_i18n_set_local(const char * lang_code);

/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const void * lv_i18n_get_text(const char * msg_id);

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const void * lv_i18n_get_text_plural(const char * msg_id, int32_t num);

/**
 * Get the name of the currently used localization.
 * @return name of the currently used localization. E.g. "en_GB"
 */
const void * lv_i18n_get_current_local(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_I18N*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LANG_H*/
