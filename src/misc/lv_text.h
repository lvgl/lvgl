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

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Options for text rendering.
 */
typedef enum {
    LV_TEXT_FLAG_NONE      = 0x00,

    /*Ignore max-width to avoid automatic word wrapping*/
    LV_TEXT_FLAG_EXPAND    = 0x01,

    /**Max-width is already equal to the longest line. (Used to skip some calculation)*/
    LV_TEXT_FLAG_FIT       = 0x02,

    /**To prevent overflow, insert breaks between any two characters.
    Otherwise breaks are inserted at word boundaries, as configured via LV_TXT_BREAK_CHARS
    or according to LV_TXT_LINE_BREAK_LONG_LEN, LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN,
    and LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN.*/
    LV_TEXT_FLAG_BREAK_ALL = 0x04,

    /**Enable parsing of recolor command*/
    LV_TEXT_FLAG_RECOLOR   = 0x08,

} lv_text_flag_t;

/** Label align policy*/
typedef enum {
    LV_TEXT_ALIGN_AUTO, /**< Align text auto*/
    LV_TEXT_ALIGN_LEFT, /**< Align text to left*/
    LV_TEXT_ALIGN_CENTER, /**< Align text to center*/
    LV_TEXT_ALIGN_RIGHT, /**< Align text to right*/
} lv_text_align_t;

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
 */
void lv_text_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font, int32_t letter_space,
                      int32_t line_space, int32_t max_width, lv_text_flag_t flag);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_H*/
