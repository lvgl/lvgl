/**
 * @file lv_ime_pinyin_private.h
 *
 */

#ifndef LV_IME_PINYIN_PRIVATE_H
#define LV_IME_PINYIN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_ime_pinyin.h"

#if LV_USE_IME_PINYIN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of lv_ime_pinyin*/
struct _lv_ime_pinyin_t {
    lv_obj_t obj;
    lv_obj_t * kb;
    lv_obj_t * cand_panel;
    const lv_pinyin_dict_t * dict;
    lv_ll_t k9_legal_py_ll;
    char * cand_str;            /* Candidate string */
    char   input_char[16];      /* Input box character */
#if LV_IME_PINYIN_USE_K9_MODE
    char   k9_input_str[LV_IME_PINYIN_K9_MAX_INPUT + 1]; /* 9-key input(k9) mode input string */
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
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_IME_PINYIN != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IME_PINYIN_PRIVATE_H*/
