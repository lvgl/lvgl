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
typedef enum
{
    LV_BIDI_DIR_LTR,
    LV_BIDI_DIR_RTL,
    LV_BIDI_DIR_NEUTRAL,
    LV_BIDI_DIR_WEAK,
}lv_bidi_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_bidi_process(const char * str_in, char * str_out, lv_bidi_dir_t base_dir);

lv_bidi_dir_t lv_bidi_get_letter_dir(uint32_t letter);
bool lv_bidi_letter_is_weak(uint32_t letter);
bool lv_bidi_letter_is_rtl(uint32_t letter);
bool lv_bidi_letter_is_neutral(uint32_t letter);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BIDI_H*/
