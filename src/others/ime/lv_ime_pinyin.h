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
#include "../../../lvgl.h"

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

/*Data of lv_ime_pinyin*/
typedef struct {
    lv_obj_t obj;
    lv_obj_t * kb;
    lv_obj_t * cand_panel;
    lv_pinyin_dict_t * dict;
    lv_ll_t k9_legal_py_ll;
    char * cand_str;            /* Candidate string */
    char   input_char[16];      /* Input box character */
#if LV_IME_PINYIN_USE_K9_MODE
    char   k9_input_str[LV_IME_PINYIN_K9_MAX_INPUT]; /* 9-key input(k9) mode input string */
    uint16_t k9_py_ll_pos;      /* Current pinyin map pages(k9) */
    uint16_t k9_legal_py_count; /* Count of legal Pinyin numbers(k9) */
    uint16_t k9_input_str_len;  /* 9-key input(k9) mode input string max len */
#endif
    uint16_t ta_count;          /* The number of characters entered in the text box this time */
    uint16_t cand_num;          /* Number of candidates */
    uint16_t py_page;           /* Current pinyin map pages(k26) */
    uint16_t py_num[26];        /* Number and length of Pinyin */
    uint16_t py_pos[26];        /* Pinyin position */
    lv_ime_pinyin_mode_t  mode; /* Set mode, 1: 26-key input(k26), 0: 9-key input(k9). Default: 1. */
} lv_ime_pinyin_t;

/***********************
 * GLOBAL VARIABLES
 ***********************/

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
 * @param dict pointer to a Pinyin input method keyboard
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
lv_pinyin_dict_t * lv_ime_pinyin_get_dict(lv_obj_t * obj);

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