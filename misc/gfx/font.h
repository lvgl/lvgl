/**
 * @file font.h
 * 
 */

#ifndef FONT_H
#define FONT_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_FONT != 0

#include <stdint.h>
#include <stddef.h>

#include "fonts/symbol_def.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _font_struct;

typedef struct _font_struct
{
    uint32_t first_ascii;
    uint32_t last_ascii;
    uint8_t height_row;
    const uint8_t * bitmap;
    const uint32_t * map;
    const uint8_t * width;
    struct _font_struct * next_page;    /*Pointer to a font extension*/
}font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the built-in fonts
 */
void font_init(void);

/**
 * Create a pair from font name and font dsc. get function. After it 'font_get' can be used for this font
 * @param child pointer to a font to join to the 'parent'
 * @param parent pointer to a font. 'child' will be joined here
 */
void font_add(font_t *child, font_t *parent);

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * font_get_bitmap(const font_t * font_p, uint32_t letter);

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
 * Get the height of a font. Give the real size on the screen (half size if FONT_ANTIALIAS is enabled)
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t font_get_height_scale(const font_t * font_p)
{
    return font_p->height_row >> FONT_ANTIALIAS;
}


/**
 * Get the width of a letter in a font
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t font_get_width(const font_t * font_p, uint32_t letter);

/**
 * Get the width of a letter in a font )Give the real size on the screen (half size if FONT_ANTIALIAS is enabled)
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t font_get_width_scale(const font_t * font_p, uint32_t letter);

/**********************
 *      MACROS
 **********************/

/***********************
 *   POST INCLUDES
 ***********************/
/*Add built-in fonts*/

#include "fonts/dejavu_10.h"
#include "fonts/dejavu_10_sup.h"
#include "fonts/dejavu_10_latin_ext_a.h"
#include "fonts/dejavu_10_latin_ext_b.h"
#include "fonts/dejavu_10_cyrillic.h"
#include "fonts/symbol_10_basic.h"
#include "fonts/symbol_10_file.h"
#include "fonts/symbol_10_feedback.h"

#include "fonts/dejavu_20.h"
#include "fonts/dejavu_20_sup.h"
#include "fonts/dejavu_20_latin_ext_a.h"
#include "fonts/dejavu_20_latin_ext_b.h"
#include "fonts/dejavu_20_cyrillic.h"
#include "fonts/symbol_20_basic.h"
#include "fonts/symbol_20_file.h"
#include "fonts/symbol_20_feedback.h"

#include "fonts/dejavu_30.h"
#include "fonts/dejavu_30_sup.h"
#include "fonts/dejavu_30_latin_ext_a.h"
#include "fonts/dejavu_30_latin_ext_b.h"
#include "fonts/dejavu_30_cyrillic.h"
#include "fonts/symbol_30_basic.h"
#include "fonts/symbol_30_file.h"
#include "fonts/symbol_30_feedback.h"

#include "fonts/dejavu_40.h"
#include "fonts/dejavu_40_sup.h"
#include "fonts/dejavu_40_latin_ext_a.h"
#include "fonts/dejavu_40_latin_ext_b.h"
#include "fonts/dejavu_40_cyrillic.h"
#include "fonts/symbol_40_basic.h"
#include "fonts/symbol_40_file.h"
#include "fonts/symbol_40_feedback.h"

#include "fonts/dejavu_60.h"
#include "fonts/dejavu_60_sup.h"
#include "fonts/dejavu_60_latin_ext_a.h"
#include "fonts/dejavu_60_latin_ext_b.h"
#include "fonts/dejavu_60_cyrillic.h"
#include "fonts/symbol_60_basic.h"
#include "fonts/symbol_60_file.h"
#include "fonts/symbol_60_feedback.h"

#include "fonts/dejavu_80.h"
#include "fonts/dejavu_80_sup.h"
#include "fonts/dejavu_80_latin_ext_a.h"
#include "fonts/dejavu_80_latin_ext_b.h"
#include "fonts/dejavu_80_cyrillic.h"
#include "fonts/symbol_80_basic.h"
#include "fonts/symbol_80_file.h"
#include "fonts/symbol_80_feedback.h"

#endif /*USE_FONT*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
