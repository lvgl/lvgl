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

/**********************
 *      TYPEDEFS
 **********************/

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
 * @param max_width max with of the text (break the lines to fit this size) Set LV_CORD_MAX to avoid line breaks
 */
void txt_get_size(point_t * size_res, const char * text, const font_t * font,
                    uint16_t letter_space, uint16_t line_space, cord_t max_width);

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param max_l max line length
 * @return the index of the first char of the new line
 */
uint16_t txt_get_next_line(const char * txt, const font_t * font_p,
                            uint16_t letter_space, cord_t max_l);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param char_num number of characters in 'txt'
 * @param font_p pointer to a font
 * @param letter_space letter sapce
 * @return length of a char_num long text
 */
cord_t txt_get_width(const char * txt, uint16_t char_num,
                    const font_t * font_p, uint16_t letter_space);

/**********************
 *      MACROS
 **********************/

#endif

