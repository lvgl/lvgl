/**
 * @file lv_i18n.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_i18n.h"
#if USE_LV_I18N

#include "lv_obj.h"
#include "../lv_misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const void * lv_i18n_get_text_core(const lv_i18n_trans_t * trans, const char * msg_id);

/**********************
 *  STATIC VARIABLES
 **********************/
static const lv_i18n_lang_pack_t * languages;
static const lv_i18n_lang_t * local_lang;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 * @return 0: no error; < 0: error
 */
int lv_i18n_init(const lv_i18n_lang_pack_t * langs)
{
    if(langs == NULL) {
        LV_LOG_WARN("lv_i18n_init: `langs` can't be NULL");
        return -1;
    }

    if(langs[0] == NULL) {
        LV_LOG_WARN("lv_i18n_init: `langs` need to contain at least one translation");
        return -1;
    }

    languages = langs;
    local_lang = langs[0];     /*Automatically select the first language*/

    return 0;
}

/**
 * Change the localization (language)
 * @param lang_code name of the translation to use. E.g. "en_GB"
 * @return 0: no error; < 0: error
 */
int lv_i18n_set_local(const char * lang_code)
{
    if(languages == NULL) {
        LV_LOG_WARN("lv_i18n_set_local: The languages are not set with lv_i18n_init() yet");
        return -1;
    }

    uint16_t i;
    for(i = 0; languages[i] != NULL; i++) {
        if(strcmp(languages[i]->name, lang_code) == 0)  break;  /*A language has found*/
    }

    /*The language wasn't found*/
    if(languages[i] == NULL) {
        LV_LOG_WARN("lv_i18n_set_local: The selected language doesn't found");
        return -1;
    }

    local_lang = languages[i];

    LV_LOG_INFO("lv_i18n_set_local: new local selected")

    return 0;
}

/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(local_lang == NULL) {
        LV_LOG_WARN("lv_i18n_get_text: No language selected");
        return msg_id;
    }

    const lv_i18n_lang_t * lang = local_lang;

    if(lang->simple == NULL) {
        if(lang == languages[0]) {
            LV_LOG_WARN("lv_i18n_get_text: No translations are specified even on the default language.");
            return msg_id;
        } else {
            LV_LOG_WARN("lv_i18n_get_text: No translations are specified on the current local. Fallback to the default language");
            lang = languages[0];
        }

        if(lang->simple == NULL) {
            LV_LOG_WARN("lv_i18n_get_text: No translations are specified even on the default language.");
            return msg_id;
        }
    }

    /*Find the translation*/
    const void * txt = lv_i18n_get_text_core(lang->simple, msg_id);
    if(txt == NULL) {
        if(lang == languages[0]) {
            LV_LOG_WARN("lv_i18n_get_text: No translation found even on the default language");
            return msg_id;
        } else {
            LV_LOG_WARN("lv_i18n_get_text: No translation found on this language. Fallback to the default language");
            lang = languages[0];
        }
    }

    /*Try again with the default language*/
    if(lang->simple == NULL) {
        LV_LOG_WARN("lv_i18n_get_text: No translations are specified even on the default language.");
        return msg_id;
    }

    txt = lv_i18n_get_text_core(lang->simple, msg_id);
    if(txt == NULL) {
        LV_LOG_WARN("lv_i18n_get_text: No translation found even on the default language");
        return msg_id;
    }

    return txt;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(local_lang == NULL) {
        LV_LOG_WARN("lv_i18n_get_text_plural: No language selected");
        return msg_id;
    }

    const lv_i18n_lang_t * lang = local_lang;

    if(lang->plural_rule == NULL) {
        if(lang == languages[0]) {
            LV_LOG_WARN("lv_i18n_get_text_plural: No plural rule has defined even on the default language");
            return msg_id;
        } else {
            LV_LOG_WARN("lv_i18n_get_text_plural: No plural rule has defined for the language. Fallback to the default language");
            lang = languages[0];
        }

        if(lang->plural_rule == NULL) {
            LV_LOG_WARN("lv_i18n_get_text_plural: No plural rule has defined even on the default language");
            return msg_id;
        }
    }

    lv_i18n_plural_type_t ptype = lang->plural_rule(num);

    if(lang->plurals[ptype] == NULL) {
        if(lang == languages[0]) {
            LV_LOG_WARN("lv_i18n_get_text_plural: No translations of the required plural form even on the default language.");
            return msg_id;
        } else {
            LV_LOG_WARN("lv_i18n_get_text_plural:No translations of the required plural form for the language. Fallback to the default language");
            lang = languages[0];
        }
    }

    /*Find the translation*/
    const void * txt = lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
    if(txt == NULL) {
        if(lang == languages[0]) {
            LV_LOG_WARN("lv_i18n_get_text_plural: No translation found even on the default language");
            return msg_id;
        } else {
            LV_LOG_WARN("lv_i18n_get_text_plural: No translation found on this language. Fallback to the default language");
            lang = languages[0];
        }
    }

    /*Try again with the default language*/
    if(lang->plurals == NULL || lang->plural_rule == NULL) {
        LV_LOG_WARN("lv_i18n_get_text_plural: No plurals or plural rule has defined even on the default language");
        return msg_id;
    }

    ptype = lang->plural_rule(num);
    if(lang->plurals[ptype] == NULL) {
        LV_LOG_WARN("lv_i18n_get_text_plural: No translations of the required plural form even on the default language.");
        return msg_id;
    }

    txt = lv_i18n_get_text_core(lang->plurals[ptype], msg_id);

    if(txt == NULL) {
        LV_LOG_WARN("lv_i18n_get_text_plural: No translation found even on the default language");
        return msg_id;
    }

    return txt;
}

/**
 * Get the name of the currently used localization.
 * @return name of the currently used localization. E.g. "en_GB"
 */
const char *  lv_i18n_get_current_local(void)
{
    if(local_lang) return local_lang->name;
    else return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const void * lv_i18n_get_text_core(const lv_i18n_trans_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].txt_trans) return trans[i].txt_trans;
        }
    }

    LV_LOG_TRACE("lv_i18n_get_text_core: `msg_id` wasn't found");
    return NULL;

}

#endif /*USE_LV_I18N*/
