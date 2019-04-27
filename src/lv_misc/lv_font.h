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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "lv_symbol_def.h"

/*********************
 *      DEFINES
 *********************/
/*Number of fractional digits in the advanced width (`adv_w`) field of `lv_font_glyph_dsc_t`*/
#define LV_FONT_ADV_W_FRACT_DIGIT       4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    uint32_t bitmap_index : 20;     /* Start index of the bitmap. A font can be max 1 MB. */
    uint32_t adv_w :12;              /*The glyph needs this space. Draw the next glyph after this width. 8 bit integer, 4 bit fractional */
    uint8_t box_w;              /*Width of the glyph's bounding box*/
    uint8_t box_h;              /*Height of the glyph's bounding box*/
    uint8_t ofs_x;              /*x offset of the bounding box*/
    int8_t  ofs_y;              /*y offset of the bounding box*/
} lv_font_glyph_dsc_t;

typedef struct {
    const uint8_t * glyph_bitmap;
    const lv_font_glyph_dsc_t * glyph_dsc;
    const uint16_t * unicode_list;
    uint16_t glyph_cnt;        /*Number of glyphs in the font. */
}lv_font_dsc_built_in_t;


typedef struct _lv_font_struct
{
    uint32_t unicode_first;
    uint32_t unicode_last;

    /*Get a glyph's  descriptor from a font*/
    const lv_font_glyph_dsc_t * (*get_glyph_dsc)(const struct _lv_font_struct *, uint32_t letter);

    /*Get a glyph's bitmap from a font*/
    const uint8_t * (*get_glyph_bitmap)(const struct _lv_font_struct *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    struct _lv_font_struct * next_page;
    uint8_t size;             /*The original size*/
    uint8_t line_height;      /*The real line height where any text fits*/
    uint8_t base_line;        /*Base line measured from the top of the line*/
    uint8_t bpp;              /*Bit per pixel: 1, 2, 4 or 8*/

    void * dsc;               /*Store implementation specific data here*/
} lv_font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the fonts
 */
void lv_font_init(void);

/**
 * Add a font to an other to extend the character set.
 * @param child the font to add
 * @param parent this font will be extended. Using it later will contain the characters from `child`
 */
void lv_font_add(lv_font_t * child, lv_font_t * parent);

/**
 * Remove a font from a character set.
 * @param child the font to remove
 * @param parent remove `child` from here
 */
void lv_font_remove(lv_font_t * child, lv_font_t * parent);

/**
 * Tells if font which contains `letter` is monospace or not
 * @param font_p point to font
 * @param letter an UNICODE character code
 * @return true: the letter is monospace; false not monospace
 */
bool lv_font_is_monospace(const lv_font_t * font_p, uint32_t letter);


/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_glyph_bitmap(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the description of a glyph in a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return pointer to a glyph descriptor
 */
const lv_font_glyph_dsc_t * lv_font_get_glyph_dsc(const lv_font_t * font_p, uint32_t letter);

uint8_t lv_font_get_width_int(const lv_font_t * font, uint32_t letter);
/**
 * Get the width of a letter in a font. If `monospace` is set then return with it.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return the width of a letter
 */
uint8_t lv_font_get_width_int(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the line height of a font. All characters fit into this height
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t lv_font_get_line_height(const lv_font_t * font_p)
{
    return font_p->line_height;//(uint8_t)((int16_t)font_p->ascent - font_p->descent);
}

/**
 * Get the bit-per-pixel of font
 * @param font pointer to font
 * @param letter a letter from font (font extensions can have different bpp)
 * @return bpp of the font (or font extension)
 */
uint8_t lv_font_get_bpp(const lv_font_t * font, uint32_t letter);


/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font contains all characters in
 * the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_glyph_bitmap_plain(const lv_font_t * font, uint32_t unicode_letter);

/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters
 * in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
const lv_font_glyph_dsc_t * lv_font_get_glyph_dsc_plain(const lv_font_t * font, uint32_t unicode_letter);

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) extern lv_font_t font_name;

#define LV_FONT_SET_ADV_W(_integer, _fract) ((_integer << LV_FONT_ADV_W_FRACT_DIGIT) + _fract)
#define LV_FONT_GET_ADV_W_INT(_adv_w)       (_adv_w >> LV_FONT_ADV_W_FRACT_DIGIT)
#define LV_FONT_GET_ADV_W_FRACT(_adv_w)     (_adv_w & ((1 << LV_FONT_ADV_W_FRACT_DIGIT) -1))


/**********************
 * ADD BUILT IN FONTS
 **********************/
#include "../lv_fonts/lv_font_builtin.h"

/*Declare the custom (user defined) fonts*/
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_FONT*/
