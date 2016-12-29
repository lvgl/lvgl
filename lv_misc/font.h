/**
 * @file font.h
 * 
 */

#ifndef FONT_H
#define FONT_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
#if USE_FONT_DEJAVU_8 != 0
    FONT_DEJAVU_8,
#endif
#if USE_FONT_DEJAVU_10 != 0
    FONT_DEJAVU_10,
#endif
#if USE_FONT_DEJAVU_14 != 0
    FONT_DEJAVU_14,
#endif
#if USE_FONT_DEJAVU_20 != 0
    FONT_DEJAVU_20,
#endif
#if USE_FONT_DEJAVU_30 != 0
    FONT_DEJAVU_30,
#endif
#if USE_FONT_DEJAVU_40 != 0
    FONT_DEJAVU_40,
#endif
#if USE_FONT_DEJAVU_60 != 0
    FONT_DEJAVU_60,
#endif
#if USE_FONT_DEJAVU_80 != 0
    FONT_DEJAVU_80,
#endif
    FONT_TYPE_NUM,
}font_types_t;

typedef struct
{
    uint8_t letter_cnt;
    uint8_t start_ascii;
    uint8_t width_byte;
    uint8_t height_row;
    uint8_t fixed_width;
    const uint8_t * width_bit_a;
    const uint8_t * bitmaps_a;
}font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const font_t * font_get(font_types_t letter);

/**********************
 *      MACROS
 **********************/

/**
 * Return with the bitmap of a font. 
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
static inline const uint8_t * font_get_bitmap(const font_t * font_p, uint8_t letter)
{
    if(letter < font_p->start_ascii || letter >= font_p->start_ascii + font_p->letter_cnt) return NULL;

    uint32_t index = (letter - font_p->start_ascii) * font_p->height_row * font_p->width_byte;
    return &font_p->bitmaps_a[index];
}

/**
 * Get the height of a font
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t font_get_height(const font_t * font_p)
{
    return font_p->height_row;
}

/**
 * Get the width of a letter in a font
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
static inline uint8_t font_get_width(const font_t * font_p, uint8_t letter)
{
    if(letter < font_p->start_ascii) return 0;
    
    letter -= font_p->start_ascii;
    uint8_t w = 0;
    if(letter < font_p->letter_cnt) {   
        w = font_p->fixed_width != 0 ? font_p->fixed_width : 
                                      font_p->width_bit_a[letter];
    }

    return w;
}

#endif
