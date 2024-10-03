/**
 * @file lv_text.h
 *
 */

#ifndef LV_TEXT_H
#define LV_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "lv_types.h"
#include "lv_area.h"
#include "../font/lv_font.h"
#include "../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_TXT_COLOR_CMD
#define LV_TXT_COLOR_CMD "#"
#endif

#define LV_TXT_ENC_UTF8 1
#define LV_TXT_ENC_ASCII 2

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Options for text rendering.
 */

typedef enum {
    LV_TEXT_FLAG_NONE      = 0x00,
    LV_TEXT_FLAG_EXPAND    = 0x01, /**< Ignore max-width to avoid automatic word wrapping*/
    LV_TEXT_FLAG_FIT       = 0x02, /**< Max-width is already equal to the longest line. (Used to skip some calculation)*/
    LV_TEXT_FLAG_BREAK_ALL = 0x04, /**< To prevent overflow, insert breaks between any two characters.
                                        Otherwise breaks are inserted at word boundaries, as configured via LV_TXT_BREAK_CHARS
                                        or according to LV_TXT_LINE_BREAK_LONG_LEN, LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN,
                                        and LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN.*/
    LV_TEXT_FLAG_RECOLOR   = 0x08, /**< Enable parsing of recolor command*/
} lv_text_flag_t;

/** Label align policy*/
typedef enum {
    LV_TEXT_ALIGN_AUTO, /**< Align text auto*/
    LV_TEXT_ALIGN_LEFT, /**< Align text to left*/
    LV_TEXT_ALIGN_CENTER, /**< Align text to center*/
    LV_TEXT_ALIGN_RIGHT, /**< Align text to right*/
} lv_text_align_t;

/** State machine for text renderer. */
typedef enum {
    LV_TEXT_CMD_STATE_WAIT, /**< Waiting for command*/
    LV_TEXT_CMD_STATE_PAR,  /**< Processing the parameter*/
    LV_TEXT_CMD_STATE_IN,   /**< Processing the command*/
} lv_text_cmd_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pointer to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param max_width max width of the text (break the lines to fit this size). Set COORD_MAX to avoid
 * @param flag settings for the text from ::lv_text_flag_t

 * line breaks
 */
void lv_text_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font, int32_t letter_space,
                      int32_t line_space, int32_t max_width, lv_text_flag_t flag);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in byte count and not characters (Á is 1 character but 2 bytes in
 * UTF-8)
 * @param font pointer to a font
 * @param letter_space letter space
 * @return length of a char_num long text
 */
int32_t lv_text_get_width(const char * txt, uint32_t length, const lv_font_t * font, int32_t letter_space);

/**
 * Give the length of a text with a given font with text flags
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in byte count and not characters (Á is 1 character but 2 bytes in
 * UTF-8)
 * @param font pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from ::lv_text_flag_t
 * @return length of a char_num long text
 */
int32_t lv_text_get_width_with_flags(const char * txt, uint32_t length, const lv_font_t * font, int32_t letter_space,
                                     lv_text_flag_t flags);

/**
 * Check if c is command state
 * @param state
 * @param c
 * @return True if c is state
 */
bool lv_text_is_cmd(lv_text_cmd_state_t * state, uint32_t c);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_H*/
