/**
 * @file lv_bidi_private.h
 *
 */

#ifndef LV_BIDI_PRIVATE_H
#define LV_BIDI_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_bidi.h"
#if LV_USE_BIDI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert a text to get the characters in the correct visual order according to
 * Unicode Bidirectional Algorithm
 * @param str_in the text to process
 * @param str_out store the result here. Has the be `strlen(str_in)` length
 * @param base_dir `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 */
void lv_bidi_process(const char * str_in, char * str_out, lv_base_dir_t base_dir);

/**
 * Auto-detect the direction of a text based on the first strong character
 * @param txt the text to process
 * @return `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 */
lv_base_dir_t lv_bidi_detect_base_dir(const char * txt);

/**
 * Get the logical position of a character in a line
 * @param str_in the input string. Can be only one line.
 * @param bidi_txt internally the text is bidi processed which buffer can be get here.
 * If not required anymore has to freed with `lv_free()`
 * Can be `NULL` is unused
 * @param len length of the line in character count
 * @param base_dir base direction of the text: `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 * @param visual_pos the visual character position which logical position should be get
 * @param is_rtl tell the char at `visual_pos` is RTL or LTR context
 * @return the logical character position
 */
uint16_t lv_bidi_get_logical_pos(const char * str_in, char ** bidi_txt, uint32_t len, lv_base_dir_t base_dir,
                                 uint32_t visual_pos, bool * is_rtl);

/**
 * Get the visual position of a character in a line
 * @param str_in the input string. Can be only one line.
 * @param bidi_txt internally the text is bidi processed which buffer can be get here.
 * If not required anymore has to freed with `lv_free()`
 * Can be `NULL` is unused
 * @param len length of the line in character count
 * @param base_dir base direction of the text: `LV_BASE_DIR_LTR` or `LV_BASE_DIR_RTL`
 * @param logical_pos the logical character position which visual position should be get
 * @param is_rtl tell the char at `logical_pos` is RTL or LTR context
 * @return the visual character position
 */
uint16_t lv_bidi_get_visual_pos(const char * str_in, char ** bidi_txt, uint16_t len, lv_base_dir_t base_dir,
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
void lv_bidi_process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_base_dir_t base_dir,
                               uint16_t * pos_conv_out, uint16_t pos_conv_len);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BIDI_PRIVATE_H*/
