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
void txt_get_size(point_t * size_res, const char * text, const font_t * font,
		          uint16_t letter_space, uint16_t line_space, cord_t max_width);
uint16_t txt_get_next_line(const char * txt, const font_t * font_p, uint16_t letter_space, cord_t max_l);
cord_t txt_get_width(const char * txt, uint16_t char_num, const font_t * font_p, uint16_t letter_space);

/**********************
 *      MACROS
 **********************/

#endif

