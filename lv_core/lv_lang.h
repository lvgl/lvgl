/**
 * @file lv_lang.h
 *
 */

#ifndef LV_LANG_H
#define LV_LANG_H

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

#if USE_LV_MULTI_LANG

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_LANG_TXT_ID_NONE     0xFFFF /*Used to not assign any text IDs for a multi-language object.*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Change the language
 * @param lang_id the id of the
 */
void lv_lang_set(uint8_t lang_id);

/**
 * Set a function to get the texts of the set languages from a `txt_id`
 * @param fp a function pointer to get the texts
 */
void lv_lang_set_text_func(const void * (*fp)(uint16_t));

/**
 * Use the function set by `lv_lang_set_text_func` to get the `txt_id` text in the set language
 * @param txt_id an ID of the text to get
 * @return the `txt_id` txt on the set language
 */
const void * lv_lang_get_text(uint16_t txt_id);

/**
 * Return with ID of the currently selected language
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
uint8_t lv_lang_act(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_MULTI_LANG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LANG_H*/
