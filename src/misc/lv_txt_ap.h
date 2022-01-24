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
#include "../draw/lv_draw.h"

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
} /*extern "C"*/
#endif

#endif /*LV_TXT_AP_H*/
