/**
 * @file lv_txt_ap.h
 *
 */

#ifndef LV_TXT_AP_H
#define LV_TXT_AP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_txt.h"
#include "../lv_draw/lv_draw.h"

#if LV_USE_ARABIC_PERSIAN_CHARS == 1

/*********************
 *      DEFINES
 *********************/

#define LV_UNDEF_ARABIC_PERSIAN_CHARS     (UINT32_MAX)
#define LV_AP_ALPHABET_BASE_CODE          0x0622
#define LV_AP_END_CHARS_LIST              {0,0,0,0,0,{0,0}}
/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t char_offset;
    uint16_t char_end_form;
    int8_t char_begining_form_offset;
    int8_t char_middle_form_offset;
    int8_t char_isolated_form_offset;
    struct {
        uint8_t conj_to_previous;
        uint8_t conj_to_next;
    } ap_chars_conjunction;
} ap_chars_map_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint32_t _lv_txt_ap_calc_bytes_cnt(const char * txt);
void _lv_txt_ap_proc(const char * txt, char * txt_out);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_ARABIC_PERSIAN_CHARS

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TXT_AP_H*/
