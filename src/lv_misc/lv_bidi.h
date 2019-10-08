/**
 * @file lv_bifi.h
 *
 */

#ifndef LV_BIDI_H
#define LV_BIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
enum
{
    /*The first 4 values are stored in `lv_obj_t` on 2 bits*/
    LV_BIDI_DIR_LTR      = 0x00,
    LV_BIDI_DIR_RTL      = 0x01,
    LV_BIDI_DIR_AUTO     = 0x02,
    LV_BIDI_DIR_INHERIT  = 0x03,

    LV_BIDI_DIR_NEUTRAL  = 0x20,
    LV_BIDI_DIR_WEAK     = 0x21,
};

typedef uint8_t lv_bidi_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_USE_BIDI

void lv_bidi_process(const char * str_in, char * str_out, lv_bidi_dir_t base_dir);
lv_bidi_dir_t lv_bidi_detect_base_dir(const char * txt);
lv_bidi_dir_t lv_bidi_get_letter_dir(uint32_t letter);
bool lv_bidi_letter_is_weak(uint32_t letter);
bool lv_bidi_letter_is_rtl(uint32_t letter);
bool lv_bidi_letter_is_neutral(uint32_t letter);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BIDI_H*/
