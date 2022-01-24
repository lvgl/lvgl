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

#include <stdbool.h>
#include <stdint.h>
#include "lv_txt.h"

/*********************
 *      DEFINES
 *********************/
/*Special non printable strong characters.
 *They can be inserted to texts to affect the run's direction*/
#define LV_BIDI_LRO  "\xE2\x80\xAD" /*U+202D*/
#define LV_BIDI_RLO  "\xE2\x80\xAE" /*U+202E*/

/**********************
 *      TYPEDEFS
 **********************/
enum {
    LV_BASE_DIR_LTR      = 0x00,
    LV_BASE_DIR_RTL      = 0x01,
    LV_BASE_DIR_AUTO     = 0x02,

    LV_BASE_DIR_NEUTRAL  = 0x20,
    LV_BASE_DIR_WEAK     = 0x21,
};

typedef uint8_t lv_base_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_USE_BIDI

/**
 * Convert a text to get the characters in the correct visual order according to
 * Unicode Bidirectional Algorithm
 * @param str_in the text to process
 * @param str_out store the result here. Has the be `strlen(str_in)` length
 * @param base_dir `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 */
void _lv_bidi_process(const char * str_in, char * str_out, lv_base_dir_t base_dir);

/**
 * Auto-detect the direction of a text based on the first strong character
 * @param txt the text to process
 * @return `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 */
lv_base_dir_t _lv_bidi_detect_base_dir(const char * txt);

/**
 * Get the logical position of a character in a line
 * @param str_in the input string. Can be only one line.
 * @param bidi_txt internally the text is bidi processed which buffer can be get here.
 * If not required anymore has to freed with `lv_mem_free()`
 * Can be `NULL` is unused
 * @param len length of the line in character count
 * @param base_dir base direction of the text: `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 * @param visual_pos the visual character position which logical position should be get
 * @param is_rtl tell the char at `visual_pos` is RTL or LTR context
 * @return the logical character position
 */
uint16_t _lv_bidi_get_logical_pos(const char * str_in, char ** bidi_txt, uint32_t len, lv_base_dir_t base_dir,
                                  uint32_t visual_pos, bool * is_rtl);

/**
 * Get the visual position of a character in a line
 * @param str_in the input string. Can be only one line.
 * @param bidi_txt internally the text is bidi processed which buffer can be get here.
 * If not required anymore has to freed with `lv_mem_free()`
 * Can be `NULL` is unused
 * @param len length of the line in character count
 * @param base_dir base direction of the text: `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 * @param logical_pos the logical character position which visual position should be get
 * @param is_rtl tell the char at `logical_pos` is RTL or LTR context
 * @return the visual character position
 */
uint16_t _lv_bidi_get_visual_pos(const char * str_in, char ** bidi_txt, uint16_t len, lv_base_dir_t base_dir,
                                 uint32_t logical_pos, bool * is_rtl);

/**
 * Bidi process a paragraph of text
 * @param str_in the string to process
 * @param str_out store the result here
 * @param len length of the text
 * @param base_dir base dir of the text
 * @param pos_conv_out an `uint16_t` array to store the related logical position of the character.
 * Can be `NULL` is unused
 * @param pos_conv_len length of `pos_conv_out` in element count
 */
void _lv_bidi_process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_base_dir_t base_dir,
                                uint16_t * pos_conv_out, uint16_t pos_conv_len);

/**
 * Get the real text alignment from the a text alignment, base direction and a text.
 * @param align     LV_TEXT_ALIGN_..., write back the calculated align here (LV_TEXT_ALIGN_LEFT/RIGHT/CENTER)
 * @param base_dir  LV_BASE_DIR_..., write the calculated base dir here (LV_BASE_DIR_LTR/RTL)
 * @param txt       a text, used with LV_BASE_DIR_AUTO to determine the base direction
 */
void lv_bidi_calculate_align(lv_text_align_t * align, lv_base_dir_t * base_dir, const char * txt);


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
