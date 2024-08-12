/**
 * @file lv_ime_pinyin.h
 *
 */
#ifndef LV_IME_PINYIN_H
#define LV_IME_PINYIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_IME_PINYIN != 0

/*********************
 *      DEFINES
 *********************/
#define LV_IME_PINYIN_K9_MAX_INPUT  7

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_IME_PINYIN_MODE_K26,
    LV_IME_PINYIN_MODE_K9,
    LV_IME_PINYIN_MODE_K9_NUMBER,
} lv_ime_pinyin_mode_t;

/*Data of pinyin_dict*/
typedef struct {
    const char * const py;
    const char * const py_mb;
} lv_pinyin_dict_t;

/*Data of 9-key input(k9) mode*/
typedef struct {
    char py_str[7];
} ime_pinyin_k9_py_str_t;

/***********************
 * GLOBAL VARIABLES
 ***********************/

extern const lv_obj_class_t lv_ime_pinyin_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_ime_pinyin_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the keyboard of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @param kb pointer to a Pinyin input method keyboard
 */
void lv_ime_pinyin_set_keyboard(lv_obj_t * obj, lv_obj_t * kb);

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @param dict pointer to a Pinyin input method dictionary
 */
void lv_ime_pinyin_set_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict);

/**
 * Set mode, 26-key input(k26) or 9-key input(k9).
 * @param obj  pointer to a Pinyin input method object
 * @param mode   the mode from 'lv_ime_pinyin_mode_t'
 */
void lv_ime_pinyin_set_mode(lv_obj_t * obj, lv_ime_pinyin_mode_t mode);

/*=====================
 * Getter functions
 *====================*/

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin IME object
 * @return     pointer to the Pinyin IME keyboard
 */
lv_obj_t * lv_ime_pinyin_get_kb(lv_obj_t * obj);

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @return     pointer to the Pinyin input method candidate panel
 */
lv_obj_t * lv_ime_pinyin_get_cand_panel(lv_obj_t * obj);

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @return     pointer to the Pinyin input method dictionary
 */
const lv_pinyin_dict_t * lv_ime_pinyin_get_dict(lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*LV_IME_PINYIN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_IME_PINYIN*/
