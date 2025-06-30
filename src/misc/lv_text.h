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

#define LV_TEXT_LEN_MAX UINT32_MAX

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

/** State machine for text renderer. */
typedef enum {
    LV_TEXT_CMD_STATE_WAIT, /**< Waiting for command*/
    LV_TEXT_CMD_STATE_PAR,  /**< Processing the parameter*/
    LV_TEXT_CMD_STATE_IN,   /**< Processing the command*/
} lv_text_cmd_state_t;

typedef struct {
    int32_t letter_space;   /**< Letter space between letters*/
    int32_t line_space;     /**< Space between lines of text*/
    int32_t max_width;      /**< Max width of the text (break the lines to fit this size). Set COORD_MAX to avoid*/
    lv_text_flag_t text_flags;
} lv_text_attributes_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the text attributes descriptor
 * @param attributes the text attributes descriptor to initialize
 */
void lv_text_attributes_init(lv_text_attributes_t * attributes);

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pointer to font of the text
 * @param attributes the text attributes, flags for line break behaviour, spacing etc
 */
void lv_text_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font,
                      lv_text_attributes_t * attributes);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_H*/
