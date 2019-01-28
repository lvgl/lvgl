/**
 * @file lv_lang.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_lang.h"
#if USE_LV_MULTI_LANG

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
static void lang_set_core(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t lang_act = 0;
static const void * (*get_txt)(uint16_t);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Change the language
 * @param lang_id the id of the
 */
void lv_lang_set(uint8_t lang_id)
{
    lang_act = lang_id;

    lv_obj_t * i;
    LL_READ(LV_GC_ROOT(_lv_scr_ll), i) {
        i->signal_func(i, LV_SIGNAL_LANG_CHG, NULL);

        lang_set_core(i);
    }

    lang_set_core(lv_scr_act());
}

/**
 * Set a function to get the texts of the set languages from a `txt_id`
 * @param fp a function pointer to get the texts
 */
void lv_lang_set_text_func(const void * (*fp)(uint16_t))
{
    get_txt = fp;
}

/**
 * Use the function set by `lv_lang_set_text_func` to get the `txt_id` text in the set language
 * @param txt_id an ID of the text to get
 * @return the `txt_id` txt on the set language
 */
const void * lv_lang_get_text(uint16_t txt_id)
{
    if(get_txt == NULL) {
        LV_LOG_WARN("lv_lang_get_text: text_func is not specified");
        return NULL;                    /*No text_get function specified */
    }
    if(txt_id == LV_LANG_TXT_ID_NONE) {
        LV_LOG_WARN("lv_lang_get_text: attempts to get invalid text ID");
        return NULL;      /*Invalid txt_id*/
    }

    return get_txt(txt_id);
}


/**
 * Return with ID of the currently selected language
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
uint8_t lv_lang_act(void)
{
    return lang_act;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Change the language of the children. (Called recursively)
 * @param obj pointer to an object
 */
static void lang_set_core(lv_obj_t * obj)
{
    lv_obj_t * i;
    LL_READ(obj->child_ll, i) {
        i->signal_func(i, LV_SIGNAL_LANG_CHG, NULL);

        lang_set_core(i);
    }
}

#endif /*USE_LV_MULTI_LANG*/
