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
#if USE_FONT_SYMBOL_30 || USE_FONT_SYMBOL_60
#define SYMBOL_DRIVE	"a"
#define SYMBOL_FILE	"b"
#define SYMBOL_FOLDER	"c"
#define SYMBOL_DELETE	"d"
#define SYMBOL_SAVE	"e"
#define SYMBOL_EDIT	"f"
#define SYMBOL_OK	"g"
#define SYMBOL_CLOSE	"h"
#define SYMBOL_DOWN	"i"
#define SYMBOL_LEFT	"j"
#define SYMBOL_RIGHT	"k"
#define SYMBOL_UP	"l"
#define SYMBOL_BT	"m"
#define SYMBOL_THERM	"n"
#define SYMBOL_GPS	"o"
#define SYMBOL_WARN	"p"
#define SYMBOL_INFO	"q"
#define SYMBOL_BATT1	"r"
#define SYMBOL_BATT2	"s"
#define SYMBOL_BATT3	"t"
#define SYMBOL_BATT4	"u"
#define SYMBOL_BATTCH	"v"
#define SYMBOL_HELP	"w"
#define SYMBOL_POWER	"x"
#define SYMBOL_SETUP	"y"
#define SYMBOL_WIFI	"z"
#endif

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
#if USE_FONT_SYMBOL_30 != 0
    FONT_SYMBOL_30,
#endif
#if USE_FONT_SYMBOL_60 != 0
    FONT_SYMBOL_60,
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

/**
 * Get the font from its id
 * @param font_id: the id of a font (an element of font_types_t enum)
 * @return pointer to a font descriptor
 */
const font_t * font_get(font_types_t font_id);


/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * font_get_bitmap(const font_t * font_p, uint8_t letter);

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
uint8_t font_get_width(const font_t * font_p, uint8_t letter);


/**********************
 *      MACROS
 **********************/


#endif
