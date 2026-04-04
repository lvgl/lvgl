/**
 * @file lv_text_ap.h
 *
 */

#ifndef LV_TEXT_AP_H
#define LV_TEXT_AP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_text.h"
#include "lv_types.h"
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

/**
 * Calculate the number of bytes required to store the Arabic/Persian
 * processed version of a UTF-8 string, excluding the null terminator.
 * Analogous to strlen() but accounts for character form substitutions
 * (e.g. initial, medial, final, isolated forms).
 *
 * @param txt  Null-terminated UTF-8 input string. Must not be NULL.
 * @return     Number of bytes in the processed output, excluding '\0'.
 *             Allocate (lv_text_ap_strlen(txt) + 1) bytes for the output buffer.
 */
size_t lv_text_ap_strlen(const char * txt);

/**
 * Process a UTF-8 string and replace Arabic/Persian characters with their
 * correct contextual forms (isolated, initial, medial, final) based on
 * their position within each word.
 *
 * The output buffer must be pre-allocated with at least
 * (lv_text_ap_strlen(txt) + 1) bytes.
 *
 * @param txt      Null-terminated UTF-8 input string.
 * @param txt_out  Output buffer to write the processed UTF-8 string into.
 *                 Must not overlap with @p txt.
 *
 * Returns early if @p txt or @p txt_out are null
 *
 * @note Non-Arabic/Persian characters are copied to the output unchanged.
 */
void lv_text_ap_proc(const char * txt, char * txt_out);

/**********************
 *      MACROS
 **********************/

#endif // LV_USE_ARABIC_PERSIAN_CHARS

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_AP_H*/
