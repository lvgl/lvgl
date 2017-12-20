/**
 * @file lv_font.h
 * 
 */

#ifndef LV_FONT_H
#define LV_FONT_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#include <stdint.h>
#include <stddef.h>

#include "lv_fonts/lv_symbol_def.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_font_struct
{
    uint32_t first_ascii;
    uint32_t last_ascii;
    uint8_t height_row;
    const uint8_t * bitmap;
    const uint32_t * map;
    const uint8_t * width;
    struct _lv_font_struct * next_page;    /*Pointer to a font extension*/
}lv_font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the built-in fonts
 */
void lv_font_init(void);

/**
 * Create a pair from font name and font dsc. get function. After it 'font_get' can be used for this font
 * @param child pointer to a font to join to the 'parent'
 * @param parent pointer to a font. 'child' will be joined here
 */
void lv_font_add(lv_font_t *child, lv_font_t *parent);

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_bitmap(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the height of a font
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t lv_font_get_height(const lv_font_t * font_p)
{
    return font_p->height_row;
}

/**
 * Get the height of a font. Give the real size on the screen (half size if LV_FONT_ANTIALIAS is enabled)
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t lv_font_get_height_scale(const lv_font_t * font_p)
{
    return (font_p->height_row >> LV_FONT_ANTIALIAS) >> LV_ANTIALIAS;
}


/**
 * Get the width of a letter in a font
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t lv_font_get_width(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the width of a letter in a font )Give the real size on the screen (half size if LV_FONT_ANTIALIAS is enabled)
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
static inline uint8_t lv_font_get_width_scale(const lv_font_t * font_p, uint32_t letter)
{
    return (lv_font_get_width(font_p, letter) >> LV_FONT_ANTIALIAS) >> LV_ANTIALIAS;
}

/**********************
 *      MACROS
 **********************/

/***********************
 *   POST INCLUDES
 ***********************/
/*Add built-in fonts*/

#include "lv_fonts/dejavu_10.h"
#include "lv_fonts/dejavu_10_sup.h"
#include "lv_fonts/dejavu_10_latin_ext_a.h"
#include "lv_fonts/dejavu_10_latin_ext_b.h"
#include "lv_fonts/dejavu_10_cyrillic.h"
#include "lv_fonts/symbol_10_basic.h"
#include "lv_fonts/symbol_10_file.h"
#include "lv_fonts/symbol_10_feedback.h"

#include "lv_fonts/dejavu_20.h"
#include "lv_fonts/dejavu_20_sup.h"
#include "lv_fonts/dejavu_20_latin_ext_a.h"
#include "lv_fonts/dejavu_20_latin_ext_b.h"
#include "lv_fonts/dejavu_20_cyrillic.h"
#include "lv_fonts/symbol_20_basic.h"
#include "lv_fonts/symbol_20_file.h"
#include "lv_fonts/symbol_20_feedback.h"

#include "lv_fonts/dejavu_30.h"
#include "lv_fonts/dejavu_30_sup.h"
#include "lv_fonts/dejavu_30_latin_ext_a.h"
#include "lv_fonts/dejavu_30_latin_ext_b.h"
#include "lv_fonts/dejavu_30_cyrillic.h"
#include "lv_fonts/symbol_30_basic.h"
#include "lv_fonts/symbol_30_file.h"
#include "lv_fonts/symbol_30_feedback.h"

#include "lv_fonts/dejavu_40.h"
#include "lv_fonts/dejavu_40_sup.h"
#include "lv_fonts/dejavu_40_latin_ext_a.h"
#include "lv_fonts/dejavu_40_latin_ext_b.h"
#include "lv_fonts/dejavu_40_cyrillic.h"
#include "lv_fonts/symbol_40_basic.h"
#include "lv_fonts/symbol_40_file.h"
#include "lv_fonts/symbol_40_feedback.h"

#include "lv_fonts/dejavu_60.h"
#include "lv_fonts/dejavu_60_sup.h"
#include "lv_fonts/dejavu_60_latin_ext_a.h"
#include "lv_fonts/dejavu_60_latin_ext_b.h"
#include "lv_fonts/dejavu_60_cyrillic.h"
#include "lv_fonts/symbol_60_basic.h"
#include "lv_fonts/symbol_60_file.h"
#include "lv_fonts/symbol_60_feedback.h"

#include "lv_fonts/dejavu_80.h"
#include "lv_fonts/dejavu_80_sup.h"
#include "lv_fonts/dejavu_80_latin_ext_a.h"
#include "lv_fonts/dejavu_80_latin_ext_b.h"
#include "lv_fonts/dejavu_80_cyrillic.h"
#include "lv_fonts/symbol_80_basic.h"
#include "lv_fonts/symbol_80_file.h"
#include "lv_fonts/symbol_80_feedback.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_FONT*/

