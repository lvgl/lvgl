/**
 * @file text.h
 * 
 */

#ifndef TEXT_H
#define TEXT_H

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lv_misc/area.h>
#include <stdbool.h>
#include "font.h"
#include "area.h"

/*********************
 *      DEFINES
 *********************/
#define TXT_RECOLOR_CMD  '#'

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    TXT_FLAG_NONE = 0x00,
    TXT_FLAG_RECOLOR = 0x01,
}txt_flag_t;

typedef enum
{
    TXT_CMD_STATE_WAIT,
    TXT_CMD_STATE_PAR,
    TXT_CMD_STATE_IN,
}txt_cmd_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pinter to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param flags settings for the text from 'txt_flag_t' enum
 * @param max_width max with of the text (break the lines to fit this size) Set LV_CORD_MAX to avoid line breaks
 */
void txt_get_size(point_t * size_res, const char * text, const font_t * font,
                    uint16_t letter_space, uint16_t line_space, cord_t max_width, txt_flag_t flag);

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param max_l max line length
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return the index of the first char of the new line
 */
uint16_t txt_get_next_line(const char * txt, const font_t * font_p,
                            uint16_t letter_space, cord_t max_l, txt_flag_t flag);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param char_num number of characters in 'txt'
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
cord_t txt_get_width(const char * txt, uint16_t char_num,
                    const font_t * font_p, uint16_t letter_space, txt_flag_t flag);

/**
 * Check next character in a string and decide if te character is part of the command or not
 * @param state pointer to a txt_cmd_state_t variable which stores the current state of command processing
 * @param c the current character
 * @return true: the character is part of a command and should not be written,
 *         false: the character should be written
 */
bool txt_is_cmd(txt_cmd_state_t * state, char c);

/**********************
 *      MACROS
 **********************/

#endif

