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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint16_t txt_get_next_line(const char * txt, const font_t * font_p, uint16_t letter_space, cord_t max_l);
cord_t txt_get_width(const char * txt, uint16_t char_num, const font_t * font_p, uint16_t letter_space);

/**********************
 *      MACROS
 **********************/

#endif

