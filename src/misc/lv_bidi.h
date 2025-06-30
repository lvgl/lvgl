/**
 * @file lv_bidi.h
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
#include "../lv_conf_internal.h"
#include "lv_types.h"
#include "lv_text.h"

/*********************
 *      DEFINES
 *********************/
/** Special non printable strong characters.
 * They can be inserted to texts to affect the run's direction */
#define LV_BIDI_LRO  "\xE2\x80\xAD" /*U+202D*/
#define LV_BIDI_RLO  "\xE2\x80\xAE" /*U+202E*/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_BASE_DIR_LTR      = 0x00,
    LV_BASE_DIR_RTL      = 0x01,
    LV_BASE_DIR_AUTO     = 0x02,

    LV_BASE_DIR_NEUTRAL  = 0x20,
    LV_BASE_DIR_WEAK     = 0x21,
} lv_base_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_USE_BIDI

/**
 * Get the real text alignment from the a text alignment, base direction and a text.
 * @param align     LV_TEXT_ALIGN_..., write back the calculated align here (LV_TEXT_ALIGN_LEFT/RIGHT/CENTER)
 * @param base_dir  LV_BASE_DIR_..., write the calculated base dir here (LV_BASE_DIR_LTR/RTL)
 * @param txt       a text, used with LV_BASE_DIR_AUTO to determine the base direction
 */
void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt);

/**
 * Set custom neutrals string
 * @param neutrals  default " \t\n\r.,:;'\"`!?%/\\-=()[]{}<>@#&$|"
 */
void lv_bidi_set_custom_neutrals_static(const char * neutrals);

/**********************
 *      MACROS
 **********************/

#else /*LV_USE_BIDI*/
/**
 * For compatibility if LV_USE_BIDI = 0
 * Get the real text alignment from the a text alignment, base direction and a text.
 * @param align     For LV_TEXT_ALIGN_AUTO give LV_TEXT_ALIGN_LEFT else leave unchanged, write back the calculated align here
 * @param base_dir  Unused
 * @param txt       Unused
 */
static inline void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt)
{
    LV_UNUSED(txt);
    LV_UNUSED(base_dir);
    if(*align == LV_TEXT_ALIGN_AUTO) * align = LV_TEXT_ALIGN_LEFT;
}
#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BIDI_H*/
