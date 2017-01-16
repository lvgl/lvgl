/**
 * @file font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/lv_misc/fonts/symbol_30.h>
#include <stddef.h>
#include "font.h"
#include "fonts/dejavu_8.h"
#include "fonts/dejavu_10.h"
#include "fonts/dejavu_14.h"
#include "fonts/dejavu_20.h"
#include "fonts/dejavu_30.h"
#include "fonts/dejavu_40.h"
#include "fonts/dejavu_60.h"
#include "fonts/dejavu_80.h"
#include "fonts/symbol_30.h"
#include "fonts/symbol_60.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the font from its id 
 * @param font_id: the id of a font (an element of font_types_t enum)
 * @return pointer to a font descriptor
 */
const font_t * font_get(font_types_t font_id)
{
    const font_t * font_p = NULL;
    
    switch(font_id)
    {
#if USE_FONT_DEJAVU_8 != 0
        case FONT_DEJAVU_8:
            font_p = dejavu_8_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_10 != 0
        case FONT_DEJAVU_10:
            font_p = dejavu_10_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_14 != 0
        case FONT_DEJAVU_14:
            font_p = dejavu_14_get_dsc();
            break;
#endif
#if USE_FONT_DEJAVU_20 != 0
        case FONT_DEJAVU_20:
            font_p = dejavu_20_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_30 != 0
        case FONT_DEJAVU_30:
            font_p = dejavu_30_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_40 != 0
        case FONT_DEJAVU_40:
            font_p = dejavu_40_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_60 != 0
        case FONT_DEJAVU_60:
            font_p = dejavu_60_get_dsc();
            break;
#endif

#if USE_FONT_DEJAVU_80 != 0
        case FONT_DEJAVU_80:
            font_p = dejavu_80_get_dsc();
            break;
#endif

#if USE_FONT_SYMBOL_30 != 0
        case FONT_SYMBOL_30:
            font_p = symbol_30_get_dsc();
            break;
#endif

#if USE_FONT_SYMBOL_60 != 0
        case FONT_SYMBOL_60:
            font_p = symbol_60_get_dsc();
            break;
#endif
        default:
            font_p = NULL;
    }
    
    return font_p;
}

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * font_get_bitmap(const font_t * font_p, uint8_t letter)
{
    if(letter < font_p->start_ascii || letter >= font_p->start_ascii + font_p->letter_cnt) return NULL;

    uint32_t index = (letter - font_p->start_ascii) * font_p->height_row * font_p->width_byte;
    return &font_p->bitmaps_a[index];
}

/**
 * Get the width of a letter in a font
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t font_get_width(const font_t * font_p, uint8_t letter)
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

/**********************
 *   STATIC FUNCTIONS
 **********************/
         
         
